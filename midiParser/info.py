import mido
import serial
import threading


mid = mido.MidiFile('midi/maria1.mid')
# mid = mido.MidiFile('midi/botwPrincesidon.mid')
# mid = mido.MidiFile('midi/botwRiding.mid')
# mid = mido.MidiFile('midi/marioGustyGarden.mid')

for i, track in enumerate(mid.tracks):
    print('Track {}: {}'.format(i, track.name))
    for msg in track:
      pass
      # print("  " + str(msg))
      # if not msg.type == "note_on" and not msg.type == "note_off":
      # if i==1 or True:
      #   if ("channel" in dir(msg)):
      #     if not msg.channel == 0:
      #       print("contains channel " + str(msg.channel) + " in track " + str(i))
      #   if msg.type == "program_change":
      #     # print(msg.bytes())
      #     pass