@page ObjPageAttackCol AttackCol Object

@brief @copybrief ::AER_OBJECT_ATTACKCOL

**Locals**

  - `damage (double)`: Damage dealt by the attack. Capped at 10.
  - `life (double)`: Time since the attack was started. Advances even while game is paused.
  - `lifeLimit (double)`: Duration of the attack. Works weirdly for large values on alt drifter.
  - `len (double)`: Length of attack. Used only for sword bonks, not for the damage hitbox.
  - `hitWall (bool)`: Whether the sword has bonked on a wall. NG drifter only.
  - `hurtAtFrame (double)`: Time until damaging hitmask appears.
  - `damageMask (int)`: Sprite ID of damaging hitmask.

  - `heavyStrike (bool)`: Whether the attack is a Heavy Strike.
  - `crit (bool)`: Enemies killed by the attack have more gruesome death animations.
  - `myAltCompanion (int)`: Instance ID of alt companion.
  - `hits (uint)`: Number of enemies hit with attack.
  - `knockback (double)`: Knockback when hitting an enemy (units unknown, possibly distance).

  - `bCol (?)`: ? (NG drifter only)
  - `palSwap (uint?)`: colour of attack ?

  - `speedUpWhenDeflect` : ?
  - `stun` : ?
  - `hitInst` : Instance ID of something?
  - `copy` : ?
  - `shieldBreaker` : ?
  - `charHurtEffect` : ?
  - `yoff` : ?

@sa ::AER_OBJECT_ATTACKCOL
