# micropy-bacnet

A BACnet/IP packet decoder with a C parser to be called from MicroPython on an ESP32 (for now).

## Concept

BACnet is the protocol building automation controllers use to talk to each other. A BACnet/IP packet is a set of nested layers (BVLC, NPDU, APDU) carried over UDP.

The decoder lives in plain C with no dependencies, so it can be tested on a desktop and reused in other projects. MicroPython handles the network side, and calls into the C code to turn raw packet bytes into usable values.

Feat. TODO:

- [ ] Decoders for the BVLC, NPDU, and APDU layers
- [ ] BACnet tag and value decoding
- [ ] Who-Is, I-Am, and ReadProperty support
- [ ] Encoder so a device can reply
- [ ] Native MicroPython module for the ESP32
- [ ] Performance Benchmarking

## Status

Cursor is tested and finished, with plans to move to BVLC parsing

## Testing

Run `./unit_test.sh` from the repo root. Tests use [Unity](https://github.com/ThrowTheSwitch/Unity), which is included in `vendor/unity`.

## AI Assistance

I use AI (Claude) as a tutor to answer questions, and sometimes to modify/edit code, or add more test cases:

- `unit_test.sh`, `compile_flags.txt`
- `test/test_cursor.c`: 9 of the 20 unit tests
- `pdu_decoder/core/cursor.c`: cleanup of the read helpers
- Various enhancements to documentation wording/spelling and structure

## License

MIT, see [LICENSE](LICENSE).

*Note: This is not to spec*
