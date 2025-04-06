use std::ptr;
use winapi::um::winuser::{MessageBoxW, MB_OK, MB_ICONINFORMATION};
use winapi::shared::minwindef::{BOOL, DWORD, HINSTANCE, LPVOID};

// DLL_PROCESS_ATTACH = 1, DLL_PROCESS_DETACH = 0
#[unsafe(no_mangle)]
pub extern "system" fn DllMain(
    _hinst_dll: HINSTANCE,
    fdw_reason: DWORD,
    _lp_reserved: LPVOID,
) -> BOOL {
    if fdw_reason != 1 {
        return 1;
    }

	let message = "DLL attached\0".encode_utf16().collect::<Vec<u16>>();
	let title = "Message Box\0".encode_utf16().collect::<Vec<u16>>();
	
	unsafe {
		MessageBoxW(
			ptr::null_mut(), // No owner window.
			message.as_ptr(),
			title.as_ptr(),
			MB_OK | MB_ICONINFORMATION,
		);
	}
	
    1
}
