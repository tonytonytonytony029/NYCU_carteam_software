import math
import random

'''這個函式是負責產生模擬車輛繞圈行駛的 GPS 座標資料，會根據你給的參數決定路徑長度、
點的密度和圓的位置可以透過圈數、每圈點數、半徑和圓心座標來控制整體軌跡的規模與偏移，
最後會把模擬出來的座標一筆筆寫進指定的文字檔中'''
def generate_gps_data(filename, num_laps=3, points_per_lap=100, radius=1.0, center_x=0.5, center_y=0.0):
#num_laps:這個參數決定模擬的圈數，也就是車子繞圓形軌跡的次數。
#points_per_lap:這個值控制一圈中你要產生多少個點，意思就是車子每繞一圈會被取樣幾次。
#radius:車子繞圓的半徑，會決定圓的大小。
#center_x和center_y:這兩個參數決定圓心的座標。
    with open(filename, "w") as f:
        for i in range(1, num_laps * points_per_lap+1):
            angle = 2 * math.pi * i / points_per_lap
            # 這行是在算目前這個點的角度 angle，也就是它在圓周上的位置。
            # i 是現在第幾個座標點（從 1 開始），points_per_lap 是一圈要切幾個點，決定角度間距的密度。
            # math.pi 是 π，2π 就是一整圈的弧度。整體意思是把一圈平均分成 points_per_lap 份，第 i 點就對應 i / points_per_lap 的位置。
            # 比如一圈有 12 點，第 3 點的角度就是 2π × 3 / 12 = π/2，剛好在上方。這樣你就能用 angle 去算圓周上的座標。
            x = center_x + radius * math.cos(angle)
            # 當 angle = 0 時，cos(0) = 1，那 x 就是 center_x + radius，也就是最右邊的點；
            # 如果 angle = π，cos(π) = -1，那 x 就是 center_x - radius，也就是最左邊的點。
            y = center_y + radius * math.sin(angle)
            # 當 angle = 0 時，sin(0) = 0，那 y 就是 center_y，也就是圓心的 y 座標；
            # 如果 angle = π/2，sin(π/2) = 1，那 y 就是 center_y + radius，也就是圓的最上面一點。
            # 這樣搭配 cos 算出來的 x，就可以構成整個圓的軌跡。
            f.write(f"{x:.6f},{y:.6f}\n")

if __name__ == "__main__":
    generate_gps_data("gps_data.txt")