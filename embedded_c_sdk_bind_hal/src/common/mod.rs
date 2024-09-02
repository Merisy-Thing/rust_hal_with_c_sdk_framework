pub mod atomic_ring_buffer;
pub mod format;

macro_rules! ll_invoke_inner {
    ( $( $x:expr ),* ) => {
        {
            unsafe { crate::ll_api::ll_cmd::ll_invoke( $( $x as crate::ll_api::ll_cmd::InvokeParam, )* ) }
        }
    };
}
