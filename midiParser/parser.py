import mido
import serial
import threading

mid = mido.MidiFile('BEAT1R.mid')
ser = serial.Serial('/dev/cu.usbmodem14301')

msgs = []


for i, track in enumerate(mid.tracks):
    print('Track {}: {}'.format(i, track.name))
    for msg in track:
      if msg.type == "note_on":
        msgs.append(msg)
        # print(msg)

def sendnote(channel, note, velocity):
  string = 'pressNote ' + str(channel) + ' ' + str(note) + ' ' + str(velocity) + '\n'
  ser.write(string.encode())
  print(string)
  print(type(string.encode()))
  print(type(string.encode()))
  if len(msgs) > 0:
    message = msgs.pop(0)
    print(message.time)
    timer = threading.Timer(message.time/200, sendnote, (message.channel, message.note, message.velocity))
    timer.start()

print (msgs)
message = msgs.pop(0)
timer = threading.Timer(message.time/200, sendnote, (message.channel, message.note, message.velocity))
timer.start()

# ser.write(b'sine ')
# ser.write(b'pressNote 9 69 127\n')
# timer = threading.Timer(2.0, gfg)
# timer.start()
# while True:
#   continue