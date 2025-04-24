def line_crossed(x_prev, y_prev, x_curr, y_curr, x1, y1, x2, y2):
    '''這個函式是判斷一個移動區間是否有穿越過定義好的起點終點線段。
    x_prev, y_prev 是前一筆座標，x_curr, y_curr 是現在這筆座標。
    x1, y1 和 x2, y2 是設定的起終點線的兩個端點。
    演算法核心是用兩個點分別帶入外積公式，去判斷兩筆資料是否位於線段的不同側。
    如果一前一後分別落在線的兩邊，就代表這次移動有「跨越」那條線。'''
    side1 = (x_prev - x1) * (y2 - y1) - (y_prev - y1) * (x2 - x1)
    # 前一點向量(x1,y1)->(x_prev,y_prev)與目前向量(x1,y1)->(x2,y2)的外積，
    # 正負號表示點在線的哪一側。
    side2 = (x_curr - x1) * (y2 - y1) - (y_curr - y1) * (x2 - x1)#同上，並且套用在當前坐標
    return (side1 * side2 < 0)  # 兩邊乘起來小於 0，代表符號相反，也就是落在不同側

def distance_to_line(x, y, x1, y1, x2, y2):
    '''用來計算目前點到起終點線的距離，如果小於自己設定的threshold，
    代表還卡在線段附近，就不能重新判下一圈，避免誤判。
    如果車子剛好在起點線附近來回移動，就可能連續產生很多穿越點；
    這時候如果沒有距離判斷，就會在原地來回晃動就被當成繞了好幾圈，但根本沒跑的狀況。
    這個函數就是為了 reset crossing_flag，用來解鎖下一次加圈的資格，
    像是lap要累加前必須先經過的mutex lock。
    x, y 是要計算的點；x1, y1, x2, y2 是那條線段的兩端。
    dx 和 dy 是線段的向量表示，norm 是線段長度。'''
    dx = x2 - x1
    dy = y2 - y1
    norm = (dx**2 + dy**2)**0.5
    if norm == 0.0:
        # 如果兩點剛好重合，線段變成一個點，就直接回傳該點到 (x1, y1) 的距離
        return ((x - x1)**2 + (y - y1)**2)**0.5
    # 用投影公式計算點到線的垂直距離
    return abs(dy * x - dx * y + x2 * y1 - y2 * x1) / norm


def count_laps_from_file(filename, x1=0.0, y1=-1.0, x2=0.0, y2=1.0, threshold=0.5):
    # 這個函式會打開指定的 gps 資料檔，讀取所有座標，並根據是否穿越虛擬線段來判斷圈數。
    # x1~y2 是定義的起終點線，通常垂直放在 y 軸中間；threshold 是避免重複觸發用的門檻。

    with open(filename, "r") as f:
        lines = f.readlines() 
    positions = [tuple(map(float, line.strip().split(","))) for line in lines]
    # 把文字資料轉成 [(x, y), (x, y), ...]  tuple 陣列

    lap_count = 0             # 記錄目前累積幾圈
    crossing_flag = False     # 用來避免在同一區域內重複觸發
    x_prev, y_prev = positions[0]  # 第一筆資料作為初始「前一筆」點

    for x_curr, y_curr in positions[1:]:  # 從第二筆資料開始，逐筆和前一筆比，判斷是否構成跨越

        # 判斷條件：
        # 1️.有跨越虛擬線段（line_crossed = True）
        # 2️.目前 crossing_flag = False（代表上一圈已經離開線段）
        # 3️.y 座標有落在起點線定義的範圍內（避免從圓對面穿越也被誤判）
        # 如果這三個條件都成立，代表這是一個合法的加圈事件
        if (line_crossed(x_prev, y_prev, x_curr, y_curr, x1, y1, x2, y2) and
            not crossing_flag and y1 <= y_curr <= y2):

            lap_count += 1  # 加圈數。
            crossing_flag = True  # 設為 True，作為mutex交給distance_to_line()。
            print(f"Lap {lap_count} detected at pos = ({x_curr:.2f}, {y_curr:.2f})")

        elif distance_to_line(x_curr, y_curr, x1, y1, x2, y2) > threshold:
            # 如果目前座標和起點線的垂直距離超過threshold，代表已經離開起點線區域
            # 解除 crossing_flag，代表車子已經離開、可以允許下一次穿越再觸發加圈。
            crossing_flag = False
        # 更新「前一筆」資料，準備下一次比對
        x_prev, y_prev = x_curr, y_curr
    print(f"Final lap count = {lap_count}")  # 最後輸出總共圈數

if __name__ == "__main__":
    # 如果這個檔案是被直接執行，而不是 import，就會開始讀檔並執行圈數計算
    count_laps_from_file("gps_data.txt")