import mido
import serial
import threading

mid = mido.MidiFile('midi/marioOverworld.mid')
ser = serial.Serial('/dev/cu.usbmodem14301')

msgs = []

time_div = 350


for i, track in enumerate(mid.tracks):
    for msg in track:
      if (msg.type == "note_on" or msg.type == "note_off") and i == 2:
        msgs.append(msg)

def sendnote(event, channel, note, velocity):
  string = ""
  if event == "note_on" or event == "note_off":
    string = str(event) + ' ' + str(channel) + ' ' + str(note) + ' ' + str(velocity) + '\n'
  ser.write(string.encode())
  if len(msgs) > 0:
    message = msgs.pop(0)
    timer = threading.Timer(message.time/time_div, sendnote, (message.type, message.channel, message.note, message.velocity))
    timer.start()

message = msgs.pop(0)
timer = threading.Timer(message.time/time_div, sendnote, (message.type, message.channel, message.note, message.velocity))
timer.start()
