project('ikotv-ir-receiver', 'c',
  version : '0.1',
  default_options : ['warning_level=everything']
)

prefix = get_option('prefix')

configure_file(
  input: 'ikotv-usb.sh.in',
  output: 'ikotv-usb.sh',
  configuration: {
    'prefix': prefix,  # Replaces @prefix@ with the actual install prefix
  },
  install: true,
  install_dir: join_paths(prefix, 'bin')
)

configure_file(
  input: 'ikotv-usb.service.in',
  output: 'ikotv-usb.service',
  configuration: {
    'prefix': prefix,  # Replaces @prefix@ with the actual install prefix
  },
  install: true,
  install_dir: join_paths(prefix, 'lib/systemd/system')
)

executable('ikotv-ir-receiverd',
           'main.c',
           'hid_usb.c',
           install : true,
           install_dir: join_paths(prefix, 'bin')
)
