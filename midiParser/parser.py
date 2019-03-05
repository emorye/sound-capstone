import mido
import serial
import threading

mid = mido.MidiFile('mario.mid')
ser = serial.Serial('/dev/cu.usbmodem14101')

msgs = []


for i, track in enumerate(mid.tracks):
    print('Track {}: {}'.format(i, track.name))
    for msg in track:
      if (msg.type == "note_on" or msg.type == "note_off") and i == 2:
        msgs.append(msg)
        print(msg)

def sendnote(event, channel, note, velocity):
  string = ""
  if event == "note_on":
    string = 'pressNote ' + str(channel) + ' ' + str(note) + ' ' + str(velocity) + '\n'
  elif event == "note_off":
    string = 'releaseNote ' + str(channel) + ' ' + str(note) + ' ' + str(velocity) + '\n'
  ser.write(string.encode())
  if len(msgs) > 0:
    message = msgs.pop(0)
    print(message.time)
    timer = threading.Timer(message.time/6000, sendnote, (message.type, message.channel, message.note, message.velocity))
    timer.start()

# print (msgs)
message = msgs.pop(0)
timer = threading.Timer(message.time/6000, sendnote, (message.type, message.channel, message.note, message.velocity))
timer.start()

# ser.write(b'sine ')
# ser.write(b'pressNote 9 69 127\n')
# timer = threading.Timer(2.0, gfg)
# timer.start()
# while True:
#   continue