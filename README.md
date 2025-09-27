# KalaExtract

**KalaExtract** is an executable that can pass compressed or raw files to any target binary, all data is always passed at the end of the target binary so its structure remains safe. This is especially important for executables so that they don't need to be recompiled whenever you modify or add new data or remove existing data youve already passed to that target executable.

**KalaExtract** is natively supported on **Windows** and **Linux** because it mostly uses C++ standard libraries and no OS-specific libraries. Only three external headers from [KalaHeaders](https://github.com/kalakit/kalaheaders) are used and they can be found in `/include/external`.

---

### Links

[Donate on PayPal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

[Official Discord server](https://discord.gg/jkvasmTND5)

[Official Youtube channel](https://youtube.com/greenlaser)

---

## Docs

[How to build from source](docs/build_from_source.md)

[How to use](docs/spec_1_0.md)