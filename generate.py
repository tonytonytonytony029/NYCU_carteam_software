import math
import random
import sys

'''這個函式是負責產生模擬車輛繞圈行駛的 GPS 座標資料，會根據你給的參數決定路徑長度、
點的密度和圓的位置可以透過圈數、每圈點數、半徑和圓心座標來控制整體軌跡的規模與偏移，
最後會把模擬出來的座標一筆筆寫進指定的文字檔中。新增了noise參數來模擬實際GPS的抖動，
以及每圈微小的位移變化，這樣可以讓每圈的檢測點位置都略有不同，更接近真實情況。'''
def generate_gps_data(filename, num_laps=3, points_per_lap=100, radius=1.0, center_x=0.5, center_y=0.0, noise=0.03):
    #num_laps:這個參數決定模擬的圈數，也就是車子繞圓形軌跡的次數。
    #points_per_lap:這個值控制一圈中你要產生多少個點，意思就是車子每繞一圈會被取樣幾次。
    #radius:車子繞圓的半徑，會決定圓的大小。
    #center_x和center_y:這兩個參數決定圓心的座標。
    #noise:模擬GPS訊號的隨機誤差範圍，值越大代表GPS精度越差。
    
    with open(filename, "w") as f:
        for lap in range(num_laps):
            # 每一圈增加一點微小的位移，讓不同圈的軌跡略有差異
            # 隨機在 -0.05 到 0.05 之間產生微小偏移，使得每圈的路徑不完全重疊
            lap_offset_x = random.uniform(-0.05, 0.05)
            lap_offset_y = random.uniform(-0.05, 0.05)
            
            for p in range(points_per_lap):
                # 計算當前點在圓周上的角度位置
                i = lap * points_per_lap + p + 1  # 全域點的索引
                angle = 2 * math.pi * p / points_per_lap
                # 這行是在算目前這個點的角度 angle，也就是它在圓周上的位置。
                # p 是當前圈內第幾個點，points_per_lap 是一圈要切幾個點，決定角度間距的密度。
                # math.pi 是 π，2π 就是一整圈的弧度。整體意思是把一圈平均分成 points_per_lap 份。
                
                # 計算圓周上的基本位置
                x = center_x + radius * math.cos(angle) + lap_offset_x
                # 當 angle = 0 時，cos(0) = 1，那 x 就是 center_x + radius，也就是最右邊的點；
                # 如果 angle = π，cos(π) = -1，那 x 就是 center_x - radius，也就是最左邊的點。
                # 另外加上這一圈特有的小偏移量 lap_offset_x。
                y = center_y + radius * math.sin(angle) + lap_offset_y
                # 當 angle = 0 時，sin(0) = 0，那 y 就是 center_y，也就是圓心的 y 座標；
                # 如果 angle = π/2，sin(π/2) = 1，那 y 就是 center_y + radius，也就是圓的最上面一點。
                # 這樣搭配 cos 算出來的 x，就可以構成整個圓的軌跡。
                # 另外加上這一圈特有的小偏移量 lap_offset_y。
                
                # 添加隨機噪音，模擬實際GPS的誤差
                x += random.uniform(-noise, noise)
                y += random.uniform(-noise, noise)
                
                f.write(f"{x:.6f},{y:.6f}\n")
    
    print(f"已生成 {num_laps} 圈的 GPS 軌跡資料到檔案 {filename}")

if __name__ == "__main__":
    # 提供命令行參數支持
    laps = 3  # 預設圈數
    if len(sys.argv) > 1:
        try:
            laps = int(sys.argv[1])
            print(f"將生成 {laps} 圈的 GPS 軌跡")
        except ValueError:
            print("圈數必須是整數，使用預設值3圈")
    
    generate_gps_data("gps_data.txt", num_laps=laps)