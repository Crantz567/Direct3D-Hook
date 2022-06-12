# Direct3D Hook
Hooks IDirect3DDevice9::EndScene using the Detours package from Microsoft. The address of Endscene is obtained by creating a D3DDevice, dereferencing the Device pointer which gives us the address of the Virtual Method Table. Endscene is located at offset 42 in this table.

Detours compiled as x86.
