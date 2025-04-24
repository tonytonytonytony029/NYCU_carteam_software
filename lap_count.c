#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/**
 * 這個函式是判斷一個移動區間是否有穿越過定義好的起點終點線段。
 * x_prev, y_prev 是前一筆座標，x_curr, y_curr 是現在這筆座標。
 * x1, y1 和 x2, y2 是設定的起終點線的兩個端點。
 * 演算法核心是用兩個點分別帶入外積公式，去判斷兩筆資料是否位於線段的不同側。
 * 如果一前一後分別落在線的兩邊，就代表這次移動有「跨越」那條線。
 */
int line_crossed(double x_prev, double y_prev, double x_curr, double y_curr, 
                double x1, double y1, double x2, double y2) {
    double side1 = (x_prev - x1) * (y2 - y1) - (y_prev - y1) * (x2 - x1);
    // 前一點向量(x1,y1)->(x_prev,y_prev)與目前向量(x1,y1)->(x2,y2)的外積，
    // 正負號表示點在線的哪一側。
    double side2 = (x_curr - x1) * (y2 - y1) - (y_curr - y1) * (x2 - x1);
    // 同上，並且套用在當前坐標
    return (side1 * side2 < 0);  // 兩邊乘起來小於 0，代表符號相反，也就是落在不同側
}

/**
 * 用來計算目前點到起終點線的距離，如果小於自己設定的threshold，
 * 代表還卡在線段附近，就不能重新判下一圈，避免誤判。
 * 如果車子剛好在起點線附近來回移動，就可能連續產生很多穿越點；
 * 這時候如果沒有距離判斷，就會在原地來回晃動就被當成繞了好幾圈，但根本沒跑的狀況。
 * 這個函數就是為了 reset crossing_flag，用來解鎖下一次加圈的資格，
 * 像是lap要累加前必須先經過的mutex lock。
 * x, y 是要計算的點；x1, y1, x2, y2 是那條線段的兩端。
 * dx 和 dy 是線段的向量表示，norm 是線段長度。
 */
double distance_to_line(double x, double y, double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double norm = sqrt(dx*dx + dy*dy);
    
    if (norm == 0.0) {
        // 如果兩點剛好重合，線段變成一個點，就直接回傳該點到 (x1, y1) 的距離
        return sqrt((x - x1)*(x - x1) + (y - y1)*(y - y1));
    }
    
    // 用投影公式計算點到線的垂直距離
    return fabs(dy * x - dx * y + x2 * y1 - y2 * x1) / norm;
}

/**
 * 這個函式會打開指定的 gps 資料檔，讀取所有座標，並根據是否穿越虛擬線段來判斷圈數。
 * x1~y2 是定義的起終點線，通常垂直放在 y 軸中間；threshold 是避免重複觸發用的門檻。
 * 函式會傳回計算出的總圈數，如果檔案讀取失敗則返回-1。
 */
int count_laps_from_file(const char* filename, 
                        double x1, double y1, double x2, double y2, 
                        double threshold) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }
    
    // 第一次讀檔，用來計算總行數以便配置記憶體
    int line_count = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), f)) {
        line_count++;
    }
    rewind(f);  // 回到檔案開頭準備再次讀取
    
    // 配置記憶體儲存所有位置資料
    double (*positions)[2] = malloc(line_count * sizeof(*positions));
    if (!positions) {
        printf("Error: Memory allocation failed\n");
        fclose(f);
        return -1;
    }
    
    // 第二次讀檔，把資料儲存到陣列中
    int i = 0;
    while (fgets(buffer, sizeof(buffer), f) && i < line_count) {
        char* token = strtok(buffer, ",");
        positions[i][0] = atof(token);    // x 座標
        token = strtok(NULL, ",");
        positions[i][1] = atof(token);    // y 座標
        i++;
    }
    fclose(f);
    
    // 開始計算圈數
    int lap_count = 0;             // 記錄目前累積幾圈
    int crossing_flag = 0;         // 用來避免在同一區域內重複觸發 (0=False, 1=True)
    double x_prev = positions[0][0], y_prev = positions[0][1];  // 第一筆資料作為初始「前一筆」點
    
    for (i = 1; i < line_count; i++) {
        double x_curr = positions[i][0];
        double y_curr = positions[i][1];
        
        // 判斷條件：
        // 1. 有跨越虛擬線段（line_crossed = True）
        // 2. 目前 crossing_flag = False（代表上一圈已經離開線段）
        // 3. 從左到右穿越（x_prev < 0 && x_curr > 0）
        // 4. y 座標有落在起點線定義的範圍內（避免從圓對面穿越也被誤判）
        // 如果這些條件都成立，代表這是一個合法的加圈事件
        if (line_crossed(x_prev, y_prev, x_curr, y_curr, x1, y1, x2, y2) && 
            !crossing_flag && 
            x_prev < 0 && x_curr > 0 &&  // 只計算從左到右的穿越
            y_curr >= y1 && y_curr <= y2) {
                
            lap_count++;      // 加圈數
            crossing_flag = 1;  // 設為 True，作為mutex交給distance_to_line()
            printf("Lap %d detected at pos = (%.2f, %.2f)\n", 
                   lap_count, x_curr, y_curr);
        }
        
        // 如果目前座標和起點線的垂直距離超過threshold，代表已經離開起點線區域
        // 解除 crossing_flag，代表車子已經離開、可以允許下一次穿越再觸發加圈
        if (distance_to_line(x_curr, y_curr, x1, y1, x2, y2) > threshold) {
            crossing_flag = 0;  // False
        }
        
        // 更新「前一筆」資料，準備下一次比對
        x_prev = x_curr;
        y_prev = y_curr;
    }
    
    printf("Final lap count = %d\n", lap_count);  // 最後輸出總共圈數
    free(positions);  // 釋放記憶體
    return lap_count;
}

/**
 * 主函式負責解析命令列參數並調用lap計數函式
 * 預設提供垂直放置的起終點線，位於x=0軸上，y範圍為-1到1
 */
int main(int argc, char* argv[]) {
    const char* filename;
    
    // 檢查是否提供檔案路徑參數
    if (argc > 1) {
        filename = argv[1];
    } else {
        printf("Please provide a GPS data file path\n");
        return 1;
    }
    
    // 設定起終點線的預設值和閾值
    double x1 = 0.0, y1 = -1.0, x2 = 0.0, y2 = 1.0;  // 垂直線段(0,-1)到(0,1)
    double threshold = 1.0;  // 距離閾值，決定何時可以重置crossing_flag
    
    count_laps_from_file(filename, x1, y1, x2, y2, threshold);
    return 0;
}