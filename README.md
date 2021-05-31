# libaermre

Action-Event-Response (AER) Mod Runtime Environment (MRE).

*libaermre* is one half of the AER modding framework for the Linux version of
the game [*Hyper Light Drifter*](https://en.wikipedia.org/wiki/Hyper_Light_Drifter)
(the other half being [*aerpatch*](https://github.com/Foxbud/aerpatch)).

While *aerpatch* handles minimally patching the game's executable, *libaermre*
is responsible for dynamically loading mods and exposing useful C API to those
mods.

## Installation

Installation will vary according the AER UI in use.

To automatically download and install the latest stable version of *libaermre*
using [*aerman*](https://github.com/Foxbud/aerman), run

```shell
$ ./aerman.sh framework-mre-install
```

from inside the `aerman` directory.

## API

The public interface of *libaermre* follows [*Semantic Versioning*](https://semver.org/),
and its [documentation can be found here](https://fairburn.dev/aermre-api-docs/).

## Copyright

*libaermre* is released by its authors under the Apache 2.0 license.
See files `AUTHORS.txt` and `LICENSE.txt` for details.

For copyright information related to the libraries that *libaermre* uses,
see directory `ACKNOWLEDGEMENTS`.

## Special Thanks

Special thanks to Josiah Bills for help designing the API and providing valuable feedback.