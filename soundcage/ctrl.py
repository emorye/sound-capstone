import mido
import serial
import threading
import time

class Melody:
    def __init__(self, instrument, velocity, transpose, notes, durations):
        assert len(notes) == len(durations)
        self.notes = notes
        self.durations = durations
        self.velocity = velocity
        self.transpose = transpose

        self.lock = threading.RLock()
        self.time = 0
        self.next_note = 0
        self.started = False

    def start(self):
        with self.lock:
            self.started = True

    def next(self):
        with self.lock:
            self.time += 1

            if not self.started or self.done() or self.next_note == len(self.durations):
                return None

            if self.next_note == 0:
                self.next_note += 1
                return self.notes[0]

            print "NEXT {}, {}, {}".format(self.time, self.velocity, self.transpose)
            if self.time == self.durations[self.next_note]:
                self.next_note += 1
                self.time = 0
                print self.notes[self.next_note - 1], self.velocity, self.transpose
                return self.notes[self.next_note - 1]
            else:
                return None

    def restart(self):
        with self.lock:
            self.time = 0
            self.next_note = 0

    def set_velocity(self, v):
        with self.lock:
            self.velocity = v

    def set_transpose(self, t):
        with self.lock:
            self.transpose = t

    def done(self):
        with self.lock:
            return self.next_note >= len(self.notes) and self.time >= self.durations[len(self.durations) - 1]

    def get_transpose(self):
        with self.lock:
            return self.transpose

    def get_velocity(self):
        with self.lock:
            return self.velocity


def MEL1():
    return Melody(0, 50, 60, [0, 2, 4, 7, 12, 14, 12], [5, 5, 5, 5, 5, 5, 7])


class CreatureController:
    def __init__(self, port, event, channel):
        self.serial = serial.Serial(port)

        self.event = event
        self.channel = channel
        self.tempo_scale = 1

        self.run = False
        self.melodies = []

    def _message(self, note, velocity):
        if self.event == 'note_on':
            ev = 'pressNote'
        elif self.event == 'note_off':
            ev = 'releaseNote'

        return '{event} {channel} {note} {velocity}\n'.format(
                event=ev,
                channel=self.channel,
                note=note,
                velocity=velocity).encode()

    def start(self):
        self.t = threading.Thread(target=self.loop)
        self.run = True
        self.t.start()

    def join(self):
        self.run = False
        self.t.join()

    def add_melody(self, m):
        print "added melody"
        self.melodies.append(m)

    def loop(self):
        while self.run:
            for m in self.melodies:
                print "m.time " + str(m.time)
                n = m.next()
                print n
                if n is not None:
                    message = self._message(n + m.get_transpose(), m.get_velocity())
                    self.serial.write(message)
                    self.serial.flush()
                    print message
            time.sleep(0.1)
