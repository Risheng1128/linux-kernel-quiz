
import subprocess
import numpy as np

def process(datas, samples, threshold = 2):
    datas = np.array(datas)
    res = np.zeros((samples, 3))
    # 分別計算 time_origin 及 time_opt 的平均
    mean = [datas[:,0].mean(), datas[:,1].mean(), datas[:,2].mean()]
    # 分別計算 time_origin 及 time_opt 的標準差
    std = [datas[:,0].std(), datas[:,1].std(), datas[:,2].std()]
    cnt = 0 # 計算有幾組資料被捨去
    for i in range(samples):
        for j in range(runs):
            tmp = np.abs((datas[j * samples + i] - mean) / std) # 計算資料是多少標準差
            # 如果某一組的某個資料過大，整組捨去
            if tmp[0] > threshold or tmp[1] > threshold or tmp[2] > threshold:
                cnt += 1
                datas[j * samples + i] = [0, 0, 0]
            res[i] += datas[j * samples + i]
        res[i] /= (runs - cnt) # 剩下的資料取平均
        cnt = 0 # count 歸 0
    return res

if __name__ == '__main__':
    runs = 50   # 執行次數
    samples = 999 # 一次有幾個點
    datas = []

    for i in range(runs):
        # 執行採樣
        subprocess.run('sudo taskset 0x1 ./time > out', shell = True)
        # 讀取資料
        fr = open('out', 'r')
        for line in fr.readlines():
            datas.append(line.split(' ', 2))
        fr.close()
    
    datas = np.array(datas).astype(np.double)
    # 存回資料
    np.savetxt('final', process(datas, samples))
