
@page ObjPageSecondary Secondary Object

@brief @copybrief ::AER_OBJECT_SECONDARY

**Locals**

  - `uses (uint)`: How many times the gun has been fired.
  - `family (uint)`: Gun's attack type (1 = single shot, 2 = laser, 3 = spread).
  - `type (uint)`: Gun ID.
  - `upgraded (bool)`: Whether the gun's battery usage is upgraded.
  - `hits (double)`: Number of shots landed on enemies with the gun since loading the save. For spread shot guns, hitting only some of the bullets counts   as a partial hit.
  - `kills (uint)`: Number of enemies killed by the gun since loading the save.
  - `wepKills (uint)`: Number of enemies killed by the gun total.
  - `equipped (bool)`: Whether the gun is currently equipped.
  - `owner` : ?

@sa ::AER_OBJECT_SECONDARY
