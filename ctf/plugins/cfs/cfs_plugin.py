import json
import time

from lib.Global import Config
from lib.logger import logger as log
from lib.plugin_manager import Plugin, ArgTypes
from lib.args_validation import ArgsValidation
from plugins.ssh.ssh_plugin import SshController, SshConfig
from plugins.cfs.pycfs.cfs import CfsController, CfsConfig, RemoteCfsController, RemoteCfsConfig, SP0CfsController, SP0CfsConfig


class CfsPlugin(Plugin, ArgsValidation):
    def __init__(self):
        super().__init__()
        ArgsValidation.__init__(self)
        self.name = "cfs_plugin"
        self.description = "Provide CFS command/telemetry support for CTF"
        self.targets = {}
        self.initialized = False

        self.command_map = {
            "RegisterCfsLocal":
                (self.register_cfs_local,[ArgTypes.string, ArgTypes.string, ArgTypes.number, ArgTypes.number, ArgTypes.boolean, ArgTypes.string,
                                          ArgTypes.string, ArgTypes.string, ArgTypes.string, ArgTypes.boolean,
                                          ArgTypes.string, ArgTypes.string, ArgTypes.boolean, ArgTypes.boolean, ArgTypes.boolean,
                                          ArgTypes.string, ArgTypes.number, ArgTypes.number
                                          ]),
                # name, ip, cmd_udp_port, tlm_udp_port, build_cfs, CCSDS_data_dir,
                # cfs_build_dir, cfs_build_cmd=, cfs_run_dir,cfs_port_arg,
                # cfs_exe, cfs_run_args, start_cfs_on_init, cfs_debug, cfs_run_in_xterm,
                # tlm_app_choice, evs_event_msg_mid, ccsds_ver auto_run, reboot,
                # cfs_entry_point, cfs_exe_path=, cfs_startup_time
            "RegisterCfsSP0":
                (self.register_cfs_sp0,[ArgTypes.string, ArgTypes.string, ArgTypes.number, ArgTypes.number, ArgTypes.boolean, ArgTypes.string,
                                        ArgTypes.string, ArgTypes.string, ArgTypes.string, ArgTypes.boolean,
                                        ArgTypes.string, ArgTypes.string, ArgTypes.boolean, ArgTypes.boolean, ArgTypes.boolean,
                                        ArgTypes.string, ArgTypes.number, ArgTypes.number, ArgTypes.boolean, ArgTypes.boolean,
                                        ArgTypes.string, ArgTypes.string, ArgTypes.number
                                       ]),
                # name, ip, cmd_udp_port, tlm_udp_port, build_cfs, CCSDS_data_dir,
                # cfs_build_dir, cfs_build_cmd=, cfs_run_dir,cfs_port_arg,
                # cfs_exe, cfs_run_args, start_cfs_on_init, cfs_debug, cfs_run_in_xterm,
                # tlm_app_choice, evs_event_msg_mid, ccsds_ver
            "RegisterCfsSSH":
                (self.register_cfs_remote, [ArgTypes.string, ArgTypes.string, ArgTypes.number, ArgTypes.number, ArgTypes.boolean, ArgTypes.string,
                                            ArgTypes.string, ArgTypes.string, ArgTypes.string, ArgTypes.boolean,
                                            ArgTypes.string, ArgTypes.string, ArgTypes.boolean, ArgTypes.boolean, ArgTypes.boolean,
                                            ArgTypes.string, ArgTypes.number, ArgTypes.number
                                           ]),
            # name="default", cfs_build_dir=None, cfs_build_cmd=None
            "BuildCfs":
                (self.build_cfs, [ArgTypes.string, ArgTypes.string, ArgTypes.string]),
            # name="default"
            "StartCfs":
                (self.start_cfs, [ArgTypes.string]),
            # name="default"
            "EnableCfsOutput":
                (self.enable_cfs_output, [ArgTypes.string]),
            # mid, cc, args, set_length=None, name="default"
            "SendCfsCommand":
                (self.send_cfs_command,
                 [ArgTypes.cmd_mid, ArgTypes.cmd_code, ArgTypes.cmd_arg, ArgTypes.number, ArgTypes.string]),
            # mid, cc, args, set_length=None, name="default"
            "SendMultCfsCommands":
                (self.send_mult_cfs_commands,
                 [ArgTypes.cmd_mid, ArgTypes.cmd_code, ArgTypes.cmd_arg, ArgTypes.number, ArgTypes.string, ArgTypes.number]),
            # mid, cc, args, set_length=None, name="default"
            "SendInvalidLengthCfsCommand":
                (self.send_cfs_command,
                 [ArgTypes.cmd_mid, ArgTypes.cmd_code, ArgTypes.cmd_arg, ArgTypes.number, ArgTypes.string]),
            # mid, args, name="default"

            # TODO - Disable SendTlmPacket. Rethink as CTF can interface with SB.
            # "SendTlmPacket":
            #     (self.send_tlm_packet, [ArgTypes.tlm_mid, ArgTypes.cmd_arg, ArgTypes.string]),
            # mid, cc, args, name="default"

            # TODO - Disable CheckCfsCommand. Rethink as CTF can interface with SB.
            # "CheckCfsCommand":
            #     (self.check_cfs_command, [ArgTypes.cmd_mid, ArgTypes.cmd_code, ArgTypes.cmd_arg, ArgTypes.string]),
            # mid, args, name="default"

            "CheckTlmValue":
                (self.check_tlm_value, [ArgTypes.tlm_mid, ArgTypes.comparison, ArgTypes.string]),
            # app, id, msg, msg_args=None, name="default"
            "CheckEvent":
                (self.check_event,
                 [ArgTypes.string, ArgTypes.string, ArgTypes.cmd_arg, ArgTypes.string, ArgTypes.string]),
            # name="default"
            "ShutdownCfs":
                (self.shutdown_cfs, [ArgTypes.string]),
        }

        # self.verify_required_commands = ["CheckCfsCommand", "CheckTlmValue"]
        self.verify_required_commands = ["CheckTlmValue"]

        self.initialize()

    def initialize(self):
        log.info("Initialize cfs plugin")
        return True

    def register_cfs_local(self, name="", ip=None, cmd_udp_port=None, tlm_udp_port=None, build_cfs=None, CCSDS_data_dir=None,
                         cfs_build_dir=None, cfs_build_cmd=None, cfs_run_dir=None,cfs_port_arg=None,
                         cfs_exe=None, cfs_run_args=None, start_cfs_on_init=None, cfs_debug=None, cfs_run_in_xterm=None,
                         tlm_app_choice=None, evs_event_msg_mid=None, ccsds_ver=None):

        log.debug("CfsPlugin.register_cfs_local")
        status = False
        if not name:
            name = "default"

        if name in self.targets:
            log.error("CFS instance {} is already registered".format(name))
            return False

        config = CfsConfig(name)
        if ip:
            config.ip = self.validate_ip(ip)
        if cmd_udp_port:
            config.cmd_udp_port = self.validate_int(cmd_udp_port)
        if tlm_udp_port:
            config.tlm_udp_port = self.validate_int(tlm_udp_port)
        if build_cfs:
            config.build_cfs = build_cfs
        if CCSDS_data_dir:
            config.CCSDS_data_dir = self.validate_directory(CCSDS_data_dir)
        if cfs_build_dir:
            config.cfs_build_dir = self.validate_directory(cfs_build_dir)
        if cfs_build_cmd:
            config.cfs_build_cmd = cfs_build_cmd
        if cfs_run_dir:
            config.cfs_run_dir = self.validate_directory(cfs_run_dir)
        if cfs_port_arg:
            config.cfs_port_arg = cfs_port_arg
        if cfs_exe and cfs_run_dir:
            config.cfs_exe = self.validate_file(cfs_exe,cfs_run_dir)
        if cfs_run_args:
            config.cfs_run_args = cfs_run_args
        if start_cfs_on_init:
            config.start_cfs_on_init = start_cfs_on_init
        if cfs_debug:
            config.cfs_debug = cfs_debug
        if cfs_run_in_xterm:
            config.cfs_run_in_xterm = cfs_run_in_xterm
        if tlm_app_choice:
            config.tlm_app_choice = tlm_app_choice
        if evs_event_msg_mid:
            config.evs_event_msg_mid =  self.validate_int(evs_event_msg_mid)
        if ccsds_ver:
            config.ccsds_ver =  self.validate_int(ccsds_ver)

        if self.get_error_count() > 0:
            status = False
        else:
            target = CfsController(config)
            status = target.initialize()
            if status:
                self.targets[name] = target
                self.initialized = True
        return status

    def register_cfs_sp0(self, name="", ip=None, cmd_udp_port=None, tlm_udp_port=None, build_cfs=None, CCSDS_data_dir=None,
                         cfs_build_dir=None, cfs_build_cmd=None, cfs_run_dir=None,cfs_port_arg=None,
                         cfs_exe=None, cfs_run_args=None, start_cfs_on_init=None, cfs_debug=None, cfs_run_in_xterm=None,
                         tlm_app_choice=None, evs_event_msg_mid=None, ccsds_ver=None, auto_run=None, reboot=None,
                        cfs_entry_point=None, cfs_exe_path=None, cfs_startup_time=None):
        log.debug("CfsPlugin.register_cfs_sp0")
        status = False

        if not name:
            name = "default"

        if name in self.targets:
            log.error("CFS instance {} is already registered".format(name))
            return False

        config = SP0CfsConfig(name)
        if ip:
            config.ip = self.validate_ip(ip)
        if cmd_udp_port:
            config.cmd_udp_port = self.validate_int(cmd_udp_port)
        if tlm_udp_port:
            config.tlm_udp_port = self.validate_int(tlm_udp_port)
        if build_cfs:
            config.build_cfs = build_cfs
        if CCSDS_data_dir:
            config.CCSDS_data_dir = self.validate_directory(CCSDS_data_dir)
        if cfs_build_dir:
            config.cfs_build_dir = self.validate_directory(cfs_build_dir)
        if cfs_build_cmd:
            config.cfs_build_cmd = cfs_build_cmd
        if cfs_run_dir:
            config.cfs_run_dir = self.validate_directory(cfs_run_dir)
        if cfs_port_arg:
            config.cfs_port_arg = cfs_port_arg
        if cfs_exe and cfs_run_dir:
            config.cfs_exe = self.validate_file(cfs_exe,cfs_run_dir)
        if cfs_run_args:
            config.cfs_run_args = cfs_run_args
        if start_cfs_on_init:
            config.start_cfs_on_init = start_cfs_on_init
        if cfs_debug:
            config.cfs_debug = cfs_debug
        if cfs_run_in_xterm:
            config.cfs_run_in_xterm = cfs_run_in_xterm
        if tlm_app_choice:
            config.tlm_app_choice = tlm_app_choice
        if evs_event_msg_mid:
            config.evs_event_msg_mid =  self.validate_int(evs_event_msg_mid)
        if ccsds_ver:
            config.ccsds_ver =  self.validate_int(ccsds_ver)
        if auto_run:
            config.auto_run = auto_run
        if reboot:
            config.reboot = reboot
        #The cfs_exe, cfs_exe_path, cfs_entry_point, and cfs_startup_time will be validated by the SP0 plugin
        if cfs_exe:
            config.cfs_exe = cfs_exe
        if cfs_exe_path:
            config.cfs_exe_path = cfs_exe_path
        if cfs_entry_point:
            config.cfs_entry_point = cfs_entry_point
        if cfs_startup_time:
            config.cfs_startup_time = cfs_startup_time

        if self.get_error_count() > 0:
            status = False
        else:
            target = SP0CfsController(config)
            status = target.initialize()
            if status:
                self.targets[name] = target
                self.initialized = True
            else:
                log.error("Register for {} failed.".format(name))

        log.info("Register for {} finished.".format(name))
        return status

    def register_cfs_remote(self, name="", ip=None, cmd_udp_port=None, tlm_udp_port=None, build_cfs=None, CCSDS_data_dir=None,
                         cfs_build_dir=None, cfs_build_cmd=None, cfs_run_dir=None,cfs_port_arg=None,
                         cfs_exe=None, cfs_run_args=None, start_cfs_on_init=None, cfs_debug=None, cfs_run_in_xterm=None,
                         tlm_app_choice=None, evs_event_msg_mid=None, ccsds_ver=None):

        log.debug("CfsPlugin.register_cfs_remote")
        status = False
        if not name:
            log.info("No CFS Name Provided. Registering Default.")
            name = "default"

        if name in self.targets:
            log.error("CFS instance {} is already registered".format(name))
            return False
        config = RemoteCfsConfig(name)

        # TODO make explicit params for remote & local socket endpoints
        config.ip = ip.split('@')[-1]
        config.set_ctf_ip()
        config.name = name
        if cfs_run_dir:
            config.cfs_run_dir = cfs_run_dir
        if cmd_udp_port:
            config.cmd_udp_port = cmd_udp_port
        if tlm_udp_port:
            config.tlm_udp_port = tlm_udp_port
        if CCSDS_data_dir:
            config.CCSDS_data_dir = CCSDS_data_dir
        config.cfs_protocol = "ssh"
        config.set_cfs_run_cmd(cfs_exe, cfs_run_args)

        log.info("Registering CFS target {} with {} protocol".format(name, config.cfs_protocol))
        target = RemoteCfsController(config)
        # TODO trying to use DI and minimize args passthrough but this is getting messy.
        # Need to rethink how the remote connection is managed
        execution = SshController(SshConfig())
        execution.init_connection(ip)
        status = target.initialize(execution)
        if status:
            self.targets[name] = target
            self.initialized = True
        return status


    def build_cfs(self, name="", cfs_build_dir=None, cfs_build_cmd=None):

        log.debug("CfsPlugin.build_cfs")
        status = []

        if name =="":
            name_instances = list(self.targets)

        else:
            name_instances = [name]

        if (len(name_instances) == 0) or (name_instances[0] not in self.targets):
            log.error("No CFS instance Found. {}".format(("No Cfs Instance Registered" if name=="" else name)))
            return False
        for name in name_instances:
            instance = self.targets[name]
            instance.config.cfs_build_dir = cfs_build_dir
            instance.config.cfs_build_cmd = cfs_build_cmd
            status.append(instance.build_cfs())

        return all(status)

    def get_configuration(self, name):
        sections = Config.sections()
        name_instances = []
        config = None

        if name == "":
            for section in sections:
                if section.startswith("cfs_"):
                    name_instances.append(section)
        else:
            name_instances = [name]

        if (len(name_instances) == 0):
            return self.register_cfs_local()

        for name_instance in name_instances:
            cfs_protocol = Config.get(name_instance, "cfs_protocol", fallback="")
            if cfs_protocol.lower() == "local":
                config = CfsConfig(name_instance)
                self.register_cfs_local(name_instance,
                                        config.ip,
                                        config.cmd_udp_port,
                                        config.tlm_udp_port,
                                        config.build_cfs,
                                        config.CCSDS_data_dir,
                                        config.cfs_build_dir,
                                        config.cfs_build_cmd,
                                        config.cfs_run_dir,
                                        config.cfs_port_arg,
                                        config.cfs_exe,
                                        config.cfs_run_args,
                                        config.start_cfs_on_init,
                                        config.cfs_debug,
                                        config.cfs_run_in_xterm,
                                        config.tlm_app_choice,
                                        config.evs_event_msg_mid,
                                        config.ccsds_ver)
            elif cfs_protocol.lower() == "ssh":
                config = RemoteCfsConfig(name_instance)
                self.register_cfs_remote(name_instance,
                                        config.ip,
                                        config.cmd_udp_port,
                                        config.tlm_udp_port,
                                        config.build_cfs,
                                        config.CCSDS_data_dir,
                                        config.cfs_build_dir,
                                        config.cfs_build_cmd,
                                        config.cfs_run_dir,
                                        config.cfs_port_arg,
                                        config.cfs_exe,
                                        config.cfs_run_args,
                                        config.start_cfs_on_init,
                                        config.cfs_debug,
                                        config.cfs_run_in_xterm,
                                        config.tlm_app_choice,
                                        config.evs_event_msg_mid,
                                        config.ccsds_ver)
            elif cfs_protocol.lower() == "sp0":
                config = SP0CfsConfig(name_instance)
                self.register_cfs_sp0(name_instance,
                                        config.ip,
                                        config.cmd_udp_port,
                                        config.tlm_udp_port,
                                        config.build_cfs,
                                        config.CCSDS_data_dir,
                                        config.cfs_build_dir,
                                        config.cfs_build_cmd,
                                        config.cfs_run_dir,
                                        config.cfs_port_arg,
                                        config.cfs_exe,
                                        config.cfs_run_args,
                                        config.start_cfs_on_init,
                                        config.cfs_debug,
                                        config.cfs_run_in_xterm,
                                        config.tlm_app_choice,
                                        config.evs_event_msg_mid,
                                        config.ccsds_ver,
                                        config.auto_run,
                                        config.reboot,
                                        config.cfs_entry_point,
                                        config.cfs_exe_path,
                                        config.cfs_startup_time)
            else:
                log.error("No cfs_protocol specifined for {}".format(name_instance))


    def start_cfs(self, name=""):
        log.debug("CfsPlugin.startCfs")


        if(self.initialized == False):
              self.get_configuration(name)
        status = []

        if name =="":
            name_instances = list(self.targets)
        else:
            name_instances = [name]

        if (len(name_instances) == 0) or (name_instances[0] not in self.targets):
            log.error("No CFS instance Found. {}".format(("No Cfs Instance Registered" if name=="" else name)))
            return False
        for name_instance in name_instances:
            log.debug("CfsPlugin.startCfs for {}".format(name_instance))
            status.append(self.targets[name_instance].start_cfs())
        return all(status)

    def enable_cfs_output(self, name=""):

        log.debug("CfsPlugin.enable_cfs_output")
        status = []

        if name == "":
            name_instances = list(self.targets)
        else:
            name_instances = [name]

        if (len(name_instances) == 0) or (name_instances[0] not in self.targets):
            log.error("No CFS instance Found. {}".format(("No Cfs Instance Registered" if name=="" else name)))
            return False
        for name_instance in name_instances:
            log.debug("CfsPlugin.enable_cfs_output for {}".format(name_instance))
            status.append(self.targets[name_instance].enable_cfs_output())

        return all(status)

    def send_cfs_command(self, mid, cc, args, set_length=None, name="", ctype_args=False):
        #log.debug("CFS Plugin Sending Command to Target {}. Command MID: {} CC:{}, Command Args: {}".format(name, mid, cc, args))
        status = []

        if name == "":
            name_instances = list(self.targets)
            print(name_instances)
        else:
            name_instances = [name]
            print(name_instances)
        if (len(name_instances) == 0) or (name_instances[0] not in self.targets):
            log.error("No CFS instance Found. {}".format(("No Cfs Instance Registered" if name=="" else name)))
            return False
        for name_instance in name_instances:
            log.debug("CFS Plugin Sending Command to Target {}. Command MID: {} CC:{}, Command Args: {}".format(name_instance, mid, cc, args))
            status.append(self.targets[name_instance].send_cfs_command(mid, cc, args, set_length, ctype_args))

        return all(status)
        
    #Used to send the same command multiple times.  For the time being, only works with commands without arguments
    def send_mult_cfs_commands(self, mid, cc, args, set_length=None, name="", ctype_args=False, mult=None):
        #log.debug("CFS Plugin Sending Command to Target {}. Command MID: {} CC:{}, Command Args: {}".format(name, mid, cc, args))
        status = []

        if name == "":
            name_instances = list(self.targets)
        else:
            name_instances = [name]

        if (len(name_instances) == 0) or (name_instances[0] not in self.targets):
            log.error("No CFS instance Found. {}".format(("No Cfs Instance Registered" if name=="" else name)))
            return False
        i = 0  
        for name_instance in name_instances:
            while i < mult:
                log.debug("CFS Plugin Sending Command to Target {}. Command MID: {} CC:{}, Command Args: {}".format(name_instance, mid, cc, args))
                status.append(self.targets[name_instance].send_cfs_command(mid, cc, args, set_length, ctype_args))
                i = i + 1
                time.sleep(1) 
        return all(status)

    # def send_tlm_packet(self, mid, args, name=""):
    #     log.debug("CfsPlugin.send_tlm_packet")
    #     status = []
    #
    #     if name =="":
    #         name_instances = list(self.targets)
    #     else:
    #         name_instances = [name]
    #
    #     if (len(name_instances) == 0) or (name_instances[0] not in self.targets):
    #         log.error("No CFS instance Found. {}".format(("No Cfs Instance Registered" if name=="" else name)))
    #         return False
    #     for name_instance in name_instances:
    #         status.append(self.targets[name_instance].send_tlm_packet(mid, args))
    #
    #     return all(status)

    # def check_cfs_command(self, mid, cc, args, name=""):
    #     log.debug("CfsPlugin.check_cfs_command")
    #     status = []
    #
    #     if name == "":
    #         name_instances = list(self.targets)
    #     else:
    #         name_instances = [name]
    #
    #     if (len(name_instances) == 0) or (name_instances[0] not in self.targets):
    #         log.error("No CFS instance Found. {}".format(("No Cfs Instance Registered" if name=="" else name)))
    #         return False
    #     for name_instance in name_instances:
    #         status.append(self.targets[name_instance].check_cfs_command(mid, cc, args))
    #
    #     return all(status)

    def check_tlm_value(self, mid, args, name=""):
        log.debug("Polling CheckTlmValue for MID:{}, Args:{}".format(mid, json.dumps(args)))
        status = []

        if name == "":
            name_instances = list(self.targets)
        else:
            name_instances = [name]

        if (len(name_instances) == 0) or (name_instances[0] not in self.targets):
            log.error("No CFS instance Found. {}".format(("No Cfs Instance Registered" if name=="" else name)))
            return False
        for name_instance in name_instances:
            log.debug("Polling CheckTlmValue for NAME:{} MID:{}, Args:{}".format(name_instance, mid, json.dumps(args)))
            status.append(self.targets[name_instance].check_tlm_value(mid, args))

        return all(status)

    def check_event(self, app, id, msg, msg_args=None, name=""):
        log.debug("CfsPlugin.check_event")
        status = []

        if name == "":
            name_instances = list(self.targets)
        else:
            name_instances = [name]

        if (len(name_instances) == 0) or (name_instances[0] not in self.targets):
            log.error("No CFS instance Found. {}".format(("No Cfs Instance Registered" if name=="" else name)))
            return False
        for name_instance in name_instances:
            log.debug("CfsPlugin.check_eventor NAME:{} MID:{}, Args:{}".format(name_instance, msg, json.dumps(msg_args)))
            status.append(self.targets[name_instance].check_event(app, id, msg, msg_args))

        return all(status)

    def shutdown_cfs(self, name=""):
        log.debug("CfsPlugin.shutdown_cfs")
        status = []

        if name == "":
            name_instances = list(self.targets)
        else:
            name_instances = [name]

        if (len(name_instances) == 0) or (name_instances[0] not in self.targets):
            log.error("No CFS instance Found. {}".format(("No Cfs Instance Registered" if name=="" else name)))
            return False
        for name_instance in name_instances:
            status.append(self.targets[name_instance].shutdown_cfs())

        return all(status)

    def shutdown(self):
        log.debug("CfsPlugin.shutdown")
        for name in self.targets:
            log.debug("Shutting down CFS target {}".format(name))
            self.targets[name].shutdown()
        del self.targets
