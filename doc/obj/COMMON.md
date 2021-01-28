@page CommonLocals Common Locals
 
@brief Locals that occur in many different objects and represent the same thing in each case.

**All Objects**

  - `t (double)`: Time since the instance was created.
  - `xc`
  - `yc`
  - `z (double)`: Current position on the z-axis.
  - `xspd (double)`: Current speed on the x-axis.
  - `yspd (double)`: Current speed on the y-axis.
  - `zspd (double)`: Current speed on the z-axis.
  - `xDrag`
  - `yDrag`

  - `caseState (uint)`: ? 
  - `caseTime`
  - `caseDelayDir` 
  - `caseCheck`
  - `caseArg`
  - `caseDelay`
  - `caseScript`

  - `selected`
  - `editorType`
  - `editorSpawner`
  - `editorSubType`
  - `editorHidden`
  - `showScriptingMenu`
  - `layer`

  - `muteObject (bool)`: All sounds are muted if >=1
  - `lowAudioEffect`

  - `cid (uint)`: An ID (unique within the room) for a specific instance, always the same on loading the room. Sometimes not set?
  - `freeze`
  - `pause`
  - `controlsHaveBeenSet`
  - `deactivateInstance`
  - `objWindow`
  - `inherited`
  - `linkArg` 
  - `linkID`
  - `justcreated` 
  - `usesCaseChecking` 

**Some Objects**

  - `owner (int)`: ID of instance that created this one.
  - `oldx (double)`: X position on the previous frame.
  - `oldy (double)`: Y position on the previous frame.
  - `imgspd` : ?
  - `dir (double)`: Direction in degrees.
  - `life (double)`: Used to track lifetime of instances that are destroyed after a set time.

@sa AERInstanceGetHLDLocals
@sa AERInstanceGetHLDLocal
