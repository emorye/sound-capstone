import mido
import serial
import threading
import time
import sys


mid = mido.MidiFile('midi/maria1.mid')
state = {"time_div": 750, "vol_scale": 1}
inst_offset = {1: 0, 2: 1, 3: 2, 7: 3}

# mid = mido.MidiFile('midi/botwRiding.mid')
# state = {"time_div": 3000}
# inst_offset = {0: 2}

# mid = mido.MidiFile('midi/marioOverworld.mid')
# state = {"time_div": 300}
# inst_offset = {1: 1, 2: 1, 3: 1, 4: 1, 5: 1, 6: 1}
# inst_offset = {1: 0, 2: 1, 3: 2, 4: 3, 5: 4, 6: 5}

# mid = mido.MidiFile('midi/marioGustyGarden.mid')
# time_div = 250
# inst_offset = {4: 0, 13: 1}

# mid = mido.MidiFile('midi/botwPrincesidon.mid')
# time_div = 1500
# inst_offset = {0: 0, 1: 1, 2: 2, 4: 4, 5: 5}

ser = serial.Serial('/dev/cu.usbmodem14101', 500000)

msgs = {}
sums = {}

num_msg = 0

for i, track in enumerate(mid.tracks):
  for m in track:
    if i in inst_offset:
      if i in msgs and len(msgs[i]) > 0 and m.time == 0:
        msgs[i][len(msgs[i])-1][0].append(m)
      elif i in msgs:
        msgs[i].append(([m], i, sums[i] + m.time))
        sums[i] += m.time
        num_msg += 1
      else:
        msgs[i] = [([m], i, m.time)]
        sums[i] = m.time
        num_msg += 1

msg_final = []

for i in msgs:
  for j in msgs[i]:
    msg_final.append(j)

msg_final = sorted(msg_final, key=lambda x: x[2])
msg_archive = [x for x in msg_final]

def send_note():
  global state
  global msg_final
  global msg_archive
  msg_arr = msg_final.pop(0)
  msg_arr_len = 0
  string = ""
  for msg in msg_arr[0]:
    if msg.type == "note_on":
      yolo = msg.bytes()
      yolo[2] = yolo[2] * state["vol_scale"]
      string += " " + str(len(msg.bytes())) + " " + " ".join([str(x) for x in yolo])
      msg_arr_len += 1
    elif msg.type == "note_off" or msg.type == "program_change":
      string += " " + str(len(msg.bytes())) + " " + " ".join([str(x) for x in msg.bytes()])
      msg_arr_len += 1
  if msg_arr_len > 0:
    string = "raw " + str(inst_offset[msg_arr[1]]) + " " + str(msg_arr_len) + string + " "
    ser.write(string.encode())
    print(string)
  if len(msg_final) > 0:
    if msg_final[0][2] - msg_arr[2] == 0:
      send_note()
    else:
      timer_inner = threading.Timer((msg_final[0][2] - msg_arr[2])/state["time_div"], send_note)
      timer_inner.start()
  else:
    msg_final = [x for x in msg_archive]
    send_note()

ser.write("zero ".encode())
time.sleep(1)
ser.write("zero ".encode())
timer = threading.Timer(0, send_note)
timer.start()
