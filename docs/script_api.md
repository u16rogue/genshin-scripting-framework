# Genshin Impact Scripting Framework API

[Information](#Information) • [Namespaces](#Namespaces)

## Information
* You need to add a function named `on_load` on your script. This function gets called after the script is successfuly loaded into GSF
    * Doing anything before `on_load` gets called will result in an undefined behaviour as most functionalities are loaded **AFTER** the script gets loaded
    ```lua
    function on_load()
        -- do your script init here
    end
    ```

* When loading a script the following sequence occures:
    1. The script gets loaded into the lua state
    2. GSF looks up an `on_load` function in the script
    3. The API is initialized
    4. The `on_load` function is called

* Different API's are placed in different [namespace](#Namespaces) based off their intended purpose, this can also indicate that these different API's have specific permissions and purpose.

* Snake cases.

## Namespaces
**NOTE:** These namespaces are "pseudo" namespaces implemented through tables. There's a slight performance issue when you call/use the API from the "namespace" due to lua having to "resolve" these values from the "namespace". If you are calling/using an API a lot it is recommended to copy it to a local variable then use that instead.

Example:
```lua
local logging_function

function on_load()
    logging_function = gsf.log
end

function example()
    logging_function("use this instead of doing gsf.log(\"not this\")")
end
```

| Name    | Purpose                                        |
|:-------:|:----------------------------------------------:|
| [gsf](#Namespace:_GSF) | Provides internal script related API's for GSF |

### Namespace: GSF

| API     | Description                                                                                                        | Args      | Return | Permission |
|:-------:|:------------------------------------------------------------------------------------------------------------------:|:---------:|:------:|:----------:|
| gsf.log | Used for logging messages into the script's internal script log and if GSF is in debug mode, to the debug console. | (str_msg) | None   | None       |

## Callbacks
n/a