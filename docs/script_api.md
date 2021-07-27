# Genshin Impact Scripting Framework API

[Information](#Information) • [API](#API) • [Callbacks](#Callbacks)

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

## API
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

| Namespaces                        | Purpose                                        |
|:---------------------------------:|:----------------------------------------------:|
| [gsf](#gsf-Namespace)             | Provides internal script related API's for GSF |
| [win](#win-Namespace)             | Windows API and internals                      |
| [mem](#mem-Namespace)             | Process memory access                          |

## `gsf` Namespace

### `gsf.log(str_msg)`
Used for logging messages into the script's internal script log and if GSF is in debug mode, to the debug console.
* Parameters
    * str_msg - [string] Message to be logged
* Return
    * nil
* Remarks
* Permission
    * None

## `win` Namespace

### `win.find_module(module_name)`
Parses the game's LDR table list and finds the specified module.
* Parameters
    * module_name - [string] Name of the module to find.
* Return
    ```
    {
        base_address,
        size
    }
    ```
    * base_address - Contains the starting/base address of the module
    * size - Size of the module
* Remarks
    * When the module is not found, this function returns a `nil`.
    * module_name parameter will still require the `.dll` postfix as this API will use exact string matches.
* Permission
    * None

## `mem` Namespace

### `mem.ida_scan(start_adr, size, ida_pattern)`
Scans the memory starting at `start_adr` up until the max limit defined by the `size` parameter for a pattern provided in an IDA signature style.
* Parameters
    * start_adr - [pointer] Starting address of the scan.
    * size - [integer] Size limit of the scan from the starting address
    * ida_pattern - [string] IDA signature style pattern used for the scan
* Return
    * address - [pointer] Address of where the first occurence of the pattern was found.
* Remarks
* Permission
    * None

### `mem.patch(addr, byte_array)`
Patches the memory specified by `addr` with the array of bytes provided from `byte_array`.
* Parameters
    * addr - [pointer] Address in memory as to where the `byte_array` is written.
    * byte_array - [byte array] Array of bytes to be written at the specified address.
* Return
    * 0 - Successful
    * 1 - Already patched
    * 2 - Failed to change protection to `PAGE_EXECUTE_READWRITE`
    * 3 - Failed to restore original protection
* Remarks
* Permission
    * None

### `mem.read_uint(addr, prim_t_size)`
Reads the specified memory address as an unisgned integer with a specified size.
* Parameters
    * addr - [pointer] Address in memory to read from.
    * prim_t_size - [integer] Size of the primitive int type to read
        * 1 - BYTE
        * 2 - U16
        * 4 - U32
        * 8 - U64
        * 8 - Pointer
* Return
    * value - [integer] value that was read from the target address
* Remarks
* Permission
    * None

## Callbacks
n/a

<!--
### `x.x()`
x.
* Parameters
* Return
* Remarks
* Permission
    * None
-->