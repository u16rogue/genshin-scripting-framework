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
| [imgui](#imgui-Namespace)         | API wrapper to ImGui                           |
| [player](#player-Namespace)       | API for local player                           |

## `gsf` Namespace

### `gsf.import(file_path)`
Import a script to GSF
* Parameters
    * file_path - [string] File path to the lua script to import
* Return
    * result - [bool] Returns true if the operation was successful, otherwise false.
* Remarks
    * This API is only available to the designated autoexec lua script
* Permission
    * None

### `gsf.load(file_path)`
Import and load a script to GSF
* Parameters
    * file_path - [string] File path to the lua script to import and load
* Return
    * result - [bool] Returns true if the operation was successful, otherwise false.
* Remarks
    * This API is only available to the designated autoexec lua script
* Permission
    * None

### `gsf.log(str_msg)`
Used for logging messages into the script's internal script log and if GSF is in debug mode, to the debug console.
* Parameters
    * str_msg - [string] Message to be logged
* Return
* Remarks
* Permission
    * None

### `gsf.register_callback(id, callback)`
Registers a lua callback function to be called based off the provided ID.
* Parameters
    * id - [string] ID of the callback to register for.
    * callback - [function] Lua function to be called for the event id.
* Return
    * result - [boolean] Returns **true** if the callback was successfuly registered, otherwise **false**.
* Remarks
    * Check the [Callbacks](#Callbacks) section for more information regarding function signatures and ID's
* Permission
    * None

## `win` Namespace

### `win.find_module(module_name)`
Parses the game's LDR table list and finds the specified module.
* Parameters
    * module_name - [string] Name of the module to find.
* Return [table]
    ```
    {
        base_address,
        size
    }
    ```
    * base_address - [pointer] Contains the starting/base address of the module
    * size - [int] Size of the module
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
    * This function returns `nil` if there are no matches for the given signature.
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

### `mem.write_uint(addr, prim_t_size, value)`
Writes the `value` to a specified memory address
* Parameters
    * addr - [pointer] Address in memory to write.
    * prim_t_size - [integer] Size of the primitive int type to write
        * 1 - BYTE
        * 2 - U16
        * 4 - U32
        * 8 - U64
        * 8 - Pointer
    * value - [integer] Value to write
* Return
* Remarks
* Permission
    * None

### `mem.calc_rel_address_32(inst_addr, inst_operand_offset)`
Calculates the 4 byte operand relative address of the instruction to its absolute address.
* Parameters
    * inst_addr - [pointer] Address of the instruction.
    * inst_operand_offset - [integer] Offset to the operand.
* Return
    * absolute_address - [pointer] The resulting absolute address.
* Remarks
* Permission
    * None

## `imgui` Namespace
This API is only a wrapper for ImGui. Documentation for each function exists in the [ImGui repo](https://github.com/ocornut/imgui).

**NOTE:** These API must be called from an [on_imgui_draw](#on_imgui_draw) callback.

| API             | Wraps              | Translation                                                                                         |
|:---------------:|:------------------:|:---------------------------------------------------------------------------------------------------:|
| imgui.begin     | ImGui::Begin       | `imgui.begin(text)` → `ImGui::Begin(text, nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse)` |
| imgui.iend      | ImGui::End         |                                                                                                     |
| imgui.text      | ImGui::Text        | `imgui.text(text)` → `ImGui::Text(text)`                                                            |
| imgui.same_line | ImGui::SameLine    | `imgui.same_line()` → `ImGui::SameLine()`                                                           |
| imgui.button    | ImGui::Button      | `imgui.button(text)` → `ImGui::Button(text)`                                                        |
| imgui.separator | ImGui::Separator   |                                                                                                     |
| imgui.push_id   | ImGui::PushID(int) |                                                                                                     |
| imgui.pop_id    | ImGui::PopID       |                                                                                                     |

## `player` Namespace

### `player.get_map_coords()`
Obtains the current player map coordinates
* Parameters
* Return [table]
    ```
    {
        x, z
    }
    ```
    * x - [float] The player x coordinate
    * z - [float] The player z coordinate
* Remarks
* Permission
    * None

## Callbacks

### on_imgui_draw
Callback used for calling ImGUI API and ImGUI rendering.
* ID: `on_imgui_draw`
* Function signature
```lua
function foo()

end
```
* Function signature arguments
* Remarks

### dx_draw
Callback function for `ID3D11DeviceContext::Draw`
* ID: `dx_draw`
* Function signature
```lua
function foo(args)

end
```
* Function signature arguments
    * args - [table] Contains event and callback arguments
        * cancel - [bool] Cancels the callback preventing the call
        * VertexCount - [int] *See callback function*
        * StartVertexLocation - [int] *See callback function*
* Remarks

### dx_drawindexed
Callback function for `ID3D11DeviceContext::DrawIndexed`
* ID: `dx_drawindexed`
* Function signature
```lua
function foo(args)

end
```
* Function signature arguments
    * args - [table] Contains event and callback arguments
        * cancel - [bool] Cancels the callback preventing the call
        * IndexCount - [int] *See callback function*
        * StartIndexLocation - [int] *See callback function*
        * BaseVertexLocation - [int] *See callback function*
* Remarks

<!--
### `x.x()`
x.
* Parameters
* Return
* Remarks
* Permission
    * None

### 
.
* ID: ``
* Function signature
```lua
```
* Function signature arguments
* Remarks
-->