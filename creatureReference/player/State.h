#ifndef _STATE_H_
#define _STATE_H_

#include <cinttypes>

class Creature;

#define MAX_NAME_LEN 12

#define ACTIVE_STATES 3
#define AMBIENT_STATES 3

class State {
 public:
  State(Creature& creature, char* const name, const uint8_t id);
  State(const State&) = delete;

  /**
   * If your subclass dynamically allocate data (with malloc, calloc, or new),
   * you MUST override the destructor and delete it to avoid memory leaks.
   */
  virtual State& operator=(State const&) = delete;

  virtual uint8_t getNumRepeats() = 0;

  /**
   * Computes probability distribution for state transitions, generates a random
   * number, and uses this to produce the next state we should transition into.
   *
   * @returns State object for next state to transition into.
   */
  virtual State* transition();

  /**
   * Called every GLOBALS.CYCLE_TIME ms. Should not perform long actions.
   *
   * @params dt Difference in time, in milliseconds, since last call.
   */
  virtual void loop(uint32_t dt) = 0;

  /**
   * @returns the id of this state from 0x00 to 0xFF.
   */
  virtual uint8_t getId();

  /**
   * @returns name of this state, capped to 12 chars.
   */
  virtual char* getName();

  /**
   * @returns Array of relative weights for transitions into all other states based on this creature’s
   * current state (higherweights being more likely). This is W from the spec.
   *
   * Note: It is recommended you implement this by adding a static array field to your class and return
   * a pointer to that.
   */
  virtual const uint8_t* getLocalWeights() = 0;

  /**
   * @returns Array of weights which measure the affinity or aversion to large numbers of creatures. For a weight for state M > 0,
   * the likelihood of transitioning into M is proporitional to the number of creatures currently in M. For a weight < 0, the likelihood
   * is inversely proportional to the number of creatures currently in M. For positive weights, groups of creatures will tend to transition to M.
   * For negative weights, only a few creatures will be in M at the same time.
   */
  virtual int8_t* getGlobalWeights();

  /**
   * @returns This state's startle factor. A value of 1 represents no alteration to the creature's regular threshold.
   * Values over one make this state harder to startle, values under one make it easier.
   */
  virtual float getStartleFactor() = 0;

  /**
   * Called when PIR pin goes from LOW to HIGH.
   */
  virtual void PIR();

  // Packet transmitters
  /**
   * Transmit a startle packet.
   *
   * @param strength  Strength of the startle.
   * @param id  Randomly generated id for the startle.
   */
  virtual void txStartle(uint8_t strength, uint8_t id);

  // Packet receivers
  /**
   * Handle a PlaySound packet.
   *
   * @param payload Should be the sound index.
   */
  virtual bool rxPlaySound(uint8_t len, uint8_t* payload);

  /**
   * Handle a PlayEffecy packet.
   *
   * @param payload Should be the effect index.
   */
  virtual bool rxPlayEffect(uint8_t len, uint8_t* payload);

  /**
   * Handle a Startle packet.
   *
   * @param payload Should be the startle strength and id.
   */
  virtual bool rxStartle(int8_t rssi, uint8_t len, uint8_t* payload);

 protected:

  // Event handlers
  /**
   * Called when this creature is successfully startled. Should test strength vs threshold and possibly enter
   * startle state and tx a startle packet.
   *
   * @param strength    Strength of the startle (higher is stronger).
   * @param id  Unique identifier for this startle to avoid duplicates.
   */
  virtual void startled(uint8_t strength, uint8_t id);

  /** Reference to the creature this is a state in */
  Creature& _creature;

  int8_t _globalWeights[ACTIVE_STATES + AMBIENT_STATES] = { 10, 1, 10, -10, 10, -10 };

 private:
  char _name[MAX_NAME_LEN + 1];
  uint8_t _id;
};

#endif  // _STATE_H_
