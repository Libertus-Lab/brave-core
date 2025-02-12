// Copyright 2021-2023 Protocol Labs
// SPDX-License-Identifier: Apache-2.0, MIT
use anyhow::anyhow;
use cid::Cid;
use fvm_ipld_blockstore::{Block, Blockstore};
use serde::{de, ser};

use crate::{CBOR, DAG_CBOR};

/// Wrapper for database to handle inserting and retrieving ipld data with Cids
pub trait CborStore: Blockstore + Sized {
    /// Get typed object from block store by Cid.
    fn get_cbor<T>(&self, cid: &Cid) -> anyhow::Result<Option<T>>
    where
        T: de::DeserializeOwned,
    {
        if !matches!(cid.codec(), CBOR | DAG_CBOR) {
            return Err(anyhow!("{} is not CBOR or DagCBOR", cid.codec()));
        }
        match self.get(cid)? {
            Some(bz) => {
                let res = crate::from_slice(&bz)?;
                Ok(Some(res))
            }
            None => Ok(None),
        }
    }

    /// Put an object in the block store and return the Cid identifier.
    fn put_cbor<S>(&self, obj: &S, code: multihash_codetable::Code) -> anyhow::Result<Cid>
    where
        S: ser::Serialize,
    {
        let bytes = crate::to_vec(obj)?;
        self.put(
            code,
            &Block {
                codec: DAG_CBOR,
                data: &bytes,
            },
        )
    }
}

impl<T: Blockstore> CborStore for T {}
