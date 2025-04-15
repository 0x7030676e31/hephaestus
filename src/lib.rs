use std::{ptr, thread};

use tokio::time::{Duration, sleep};
use winapi::shared::minwindef::{BOOL, DWORD, HINSTANCE, LPVOID};
use winapi::um::processthreadsapi::ExitProcess;
use winapi::um::winuser::{MB_ICONINFORMATION, MB_OK, MessageBoxW};

// fdw_reason = DLL_PROCESS_ATTACH = 1, DLL_PROCESS_DETACH = 0

#[unsafe(no_mangle)]
pub extern "system" fn DllMain(
    _hinst_dll: HINSTANCE,
    fdw_reason: DWORD,
    _lp_reserved: LPVOID,
) -> BOOL {
    if fdw_reason != 1 {
        return 1;
    }

    thread::spawn(|| unsafe { main() });

    1
}

#[tokio::main]
#[allow(unsafe_op_in_unsafe_fn)]
async unsafe fn main() {
    sleep(Duration::from_secs(3)).await;

    let message = "DLL attached\0".encode_utf16().collect::<Vec<u16>>();
    let title = "Message Box\0".encode_utf16().collect::<Vec<u16>>();

    MessageBoxW(
        ptr::null_mut(), // No owner window.
        message.as_ptr(),
        title.as_ptr(),
        MB_OK | MB_ICONINFORMATION,
    );

    ExitProcess(0);
}
