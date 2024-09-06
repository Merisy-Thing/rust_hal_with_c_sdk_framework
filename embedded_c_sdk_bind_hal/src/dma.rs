use crate::ll_api::{ll_cmd::*, DmaCtrl, DmaFlags};
pub use crate::ll_api::DmaChannel;

pub trait DmaDataSize: PartialOrd {}

impl DmaDataSize for i8 {}
impl DmaDataSize for i16 {}
impl DmaDataSize for i32 {}
impl DmaDataSize for u8 {}
impl DmaDataSize for u16 {}
impl DmaDataSize for u32 {}

pub struct Config<'a, ST, DT> {
    pub src: DmaSrc<'a, ST>,
    pub dst: DmaDst<'a, DT>,
    pub dir: DmaDir,
    pub circular: bool,
}

#[derive(Debug, PartialEq, Eq)]
pub enum DmaSrc<'a, T> {
    Ref(&'a [T]),
    Addr(usize),
}

#[derive(Debug, PartialEq, Eq)]
pub enum DmaDst<'a, T> {
    Ref(&'a mut [T]),
    Addr(usize),
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum DmaDir {
    /// Memory to memory direction
    M2M,
    /// Peripheral to memory direction
    P2M,
    /// Memory to peripheral direction
    M2P,
}

impl<'a, ST, DT> Config<'a, ST, DT> {
    pub const fn new(
        src: DmaSrc<'a, ST>,
        dst: DmaDst<'a, DT>,
        dir: DmaDir,
        circular: bool,
    ) -> Self {
        Self {
            src,
            dst,
            dir,
            circular,
        }
    }

    pub fn set_src(&mut self, src: DmaSrc<'a, ST>) {
        self.src = src
    }

    pub fn set_dst(&mut self, dst: DmaDst<'a, DT>) {
        self.dst = dst
    }

    pub fn set_dir(&mut self, dir: DmaDir) {
        self.dir = dir
    }

    pub fn set_circular(&mut self, circular: bool) {
        self.circular = circular
    }

    pub fn build(&self) -> Result<DmaParam, i32> {
        let mut src_inc = false;
        let mut dst_inc = false;
        let (src_addr, src_buff_size) = match self.src {
            DmaSrc::Ref(buff) => {
                let addr = buff.as_ptr() as usize;
                let size = buff.len();
                src_inc = true;
                (addr, size)
            }
            DmaSrc::Addr(addr) => (addr, 0),
        };

        let (dst_addr, dst_buff_size) = match &self.dst {
            DmaDst::Ref(buff) => {
                let addr = (*buff).as_ptr() as usize;
                let size = buff.len();
                dst_inc = true;
                (addr, size)
            }
            DmaDst::Addr(addr) => (*addr, 0),
        };

        let src_data_size = match core::mem::size_of::<ST>() {
            1 => DmaFlags::SrcByte,
            2 => DmaFlags::SrcHalfWord,
            4 => DmaFlags::SrcWord,
            _ => return Err(-100),
        };

        let dst_data_size = match core::mem::size_of::<DT>() {
            1 => DmaFlags::DstByte,
            2 => DmaFlags::DstHalfWord,
            4 => DmaFlags::DstWord,
            _ => return Err(-200),
        };

        let dir = match self.dir {
            DmaDir::M2M => DmaFlags::MemToMem,
            DmaDir::P2M => DmaFlags::PeriphToMem,
            DmaDir::M2P => DmaFlags::MemToPeriph,
        };

        let circular = if self.circular {
            DmaFlags::CircularOn
        } else {
            DmaFlags::None
        };

        let inc = match (src_inc, dst_inc) {
            (true, true) => DmaFlags::BothInc,
            (true, false) => DmaFlags::SrcInc,
            (false, true) => DmaFlags::DstInc,
            (false, false) => DmaFlags::None,
        };

        let flags = src_data_size as u32 | dst_data_size as u32 | dir as u32 | inc as  u32 | circular as u32;

        Ok(DmaParam {
            src_addr,
            src_buff_size,
            dst_addr,
            dst_buff_size,
            flags,
        })
    }
}

pub struct DmaParam {
    src_addr: usize,
    src_buff_size: usize,
    dst_addr: usize,
    dst_buff_size: usize,
    flags: u32,
}

#[derive(Clone, Debug)]
pub struct Dma {
    ch: DmaChannel,
}

impl Dma {
    pub fn new(ch: DmaChannel) -> Self {
        Dma { ch }
    }

    pub fn init<ST, DT>(&self, config: &Config<ST, DT>, extra: Option<u32>) -> Result<(), i32> {
        let extra_flags = if let Some(extra) = extra { extra } else { 0 };
        let param = config.build()?;
        let result = ll_invoke_inner!(
            INVOKE_ID_DMA_INIT,
            self.ch,
            param.src_addr,
            param.src_buff_size,
            param.dst_addr,
            param.dst_buff_size,
            param.flags,
			extra_flags
        );
        if result == 0 {
            Ok(())
        } else {
            Err(result)
        }
    }

	pub fn start(&self) -> Result<(), i32> {
		let result = ll_invoke_inner!( INVOKE_ID_DMA_CTRL, self.ch, DmaCtrl::Start);
        if result == 0 {
            Ok(())
        } else {
            Err(result)
        }
	}
	
	pub fn stop(&self) -> Result<(), i32> {
		let result = ll_invoke_inner!( INVOKE_ID_DMA_CTRL, self.ch, DmaCtrl::Stop);
        if result == 0 {
            Ok(())
        } else {
            Err(result)
        }
	}

	pub fn wait(&self) -> Result<(), i32> {
		let result = ll_invoke_inner!( INVOKE_ID_DMA_CTRL, self.ch, DmaCtrl::Wait);
        if result == 0 {
            Ok(())
        } else {
            Err(result)
        }
	}
}

impl Drop for Dma {
    fn drop(&mut self) {
        ll_invoke_inner!(INVOKE_ID_DMA_DEINIT, self.ch);
    }
}
