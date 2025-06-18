#!/usr/bin/env python3
from pathlib import Path, PurePosixPath
import subprocess
import re
import argparse
import logging
import enum


# GLOBALS
PROJECT_DIR_PATH = Path(__file__).resolve().parent.parent
PROJECT_ADDONS_DIRS = [PROJECT_DIR_PATH / 'addons', PROJECT_DIR_PATH / 'optionals']
BLACKLISTED_ADDON_IDS = []
WORKBENCH_EXTRA_ARGS = ['-enableWARP', '-noThrow', '-noSound', '-scriptAuthorizeAll', '-silentCrashReport', '-VMErrorMode', 'fatal']
LOGGER_FORMAT = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'


class ReforgerBranch(enum.Enum):
    STABLE = enum.auto()
    EXPERIMENTAL = enum.auto()


class ParseEnum(argparse.Action):
    '''
    Helper action for parsing enum.Enum with argparse
    Based on https://stackoverflow.com/a/60750535
    Copyright (c) 2020 Tim
    CC BY-SA 4.0
    '''
    def __init__(self, **kwargs):
        self._values_map = kwargs.pop('type')
        kwargs.setdefault('choices', [value.name for value in self._values_map])
        super().__init__(**kwargs)


    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, self._values_map[values])


def run_subprocess(cmd, **kwargs):
    '''
    Runs subprocess of cmd
    '''
    logging.debug(f'Running: {cmd}')
    return subprocess.run(cmd, check=True, **kwargs)


def get_addon_id(gproj_path):
    '''
    Reads addon ID from gproj file
    '''
    try:
        with open(gproj_path, 'r') as stream:
            for line in stream:
                match = re.match(r' ID "(\w+)"', line)
                if match is None:
                    continue

                return match.group(1)
            
            raise ValueError('no match found')
    except:
        logging.critical(f'Failed to get addon ID from {gproj_path}')
        raise RuntimeError


def get_latest_tag(repo_path=None):
    '''
    Returns latest tag on the current git branch
    '''
    try:
        run_subprocess(['git', 'fetch', '--tags'], cwd=repo_path)
        result = run_subprocess(['git', 'describe', '--tags', '--abbrev=0'], capture_output=True, text=True, cwd=repo_path)
        return result.stdout.rstrip()
    except:
        logging.critical('Failed to get latest git tag')
        raise RuntimeError


def tag_to_version(tag):
    '''
    Converts git tag to version
    '''
    try:
        match = re.match(r'v(\d\.\d\.\d)', tag)
        return match.group(1)
    except:
        logging.critical(f'Failed to get version from tag "{tag}"')
        raise RuntimeError


def get_release_url(release_tag, repo_path=None):
    '''
    Gets release URL on GitHub for local repo for the given tag
    '''
    try:
        result = run_subprocess(['git', 'remote', 'get-url', 'origin'], capture_output=True, text=True, cwd=repo_path)
        path = PurePosixPath(result.stdout.rstrip().split(':', 1)[1])
        path = path.with_suffix('')
        return f'https://github.com/{path}/releases/tag/{release_tag}'
    except:
        logging.critical(f'Failed to get release url from tag "{release_tag}"')
        raise RuntimeError


def get_all_addons(addons_dirs=PROJECT_ADDONS_DIRS, blacklist=BLACKLISTED_ADDON_IDS):
    '''
    Returns list of addon IDs found in the addons_dir
    '''
    addons = []
    for addons_dir in addons_dirs:
        for gproj_path in addons_dir.glob('*/addon.gproj'):
            addon_id = get_addon_id(gproj_path)
            if addon_id not in blacklist:
                addons.append(addon_id)

    return addons


class Workbench:
    '''
    Interface for Arma Reforger Workbench
    '''
    logger = logging.getLogger('Workbench')


    def __init__(self, exe_path, addons_dirs=[], extra_args=[]):
        try:
            if not exe_path.is_file():
                raise FileNotFoundError(exe_path)
            
            self.exe_path = exe_path
            self.common_args = ['-addonsDir', ','.join(str(d) for d in addons_dirs)]
            self.common_args.extend(extra_args)
        except:
            self.logger.critical('Failed to initialize workbench')
            raise RuntimeError


    @classmethod
    def from_steam(cls, branch=ReforgerBranch.STABLE, addons_dirs=[], **kwargs):
        '''
        Initializes workbench from steam installation via window registry
        '''
        try:
            import winreg

            if branch == ReforgerBranch.STABLE:
                game_folder = 'Arma Reforger'
                tools_folder = 'Arma Reforger Tools'
            elif branch == ReforgerBranch.EXPERIMENTAL:
                game_folder = 'Arma Reforger Experimental'
                tools_folder = 'Arma Reforger Experimental Tools'

            reg = winreg.ConnectRegistry(None, winreg.HKEY_CURRENT_USER)
            key = winreg.OpenKey(reg, r'Software\Valve\Steam')
            steam_path = Path(winreg.QueryValueEx(key, "SteamPath")[0])
            winreg.CloseKey(key)
            steam_apps_path = steam_path / 'steamapps' / 'common'
            exe_path = steam_apps_path / tools_folder / 'Workbench' / 'ArmaReforgerWorkbenchSteamDiag.exe'
            addons_dirs.insert(0, steam_apps_path / game_folder / 'addons')
        except:
            cls.logger.critical('Failed to initialize workbench')
            raise RuntimeError

        return cls(exe_path, addons_dirs=addons_dirs, **kwargs)


    def run(self, addon_id, extra_args):
        '''
        Runs the workbench for the given addon and extra arguments
        '''
        args = self.common_args + ['-addons', str(addon_id), *extra_args]
        run_subprocess([str(self.exe_path), *args])


    def validate_scripts(self, addon_id):
        '''
        Validates scripts for the given addon
        '''
        try:
            self.logger.info(f'Validating {addon_id}')
            self.run(addon_id, ['-wbSilent', '-wbmodule=ScriptEditor', '-validate'])
        except:
            self.logger.critical(f'Validating scripts of {addon_id} failed')
            raise RuntimeError


    def pack_addon(self, addon_id):
        '''
        Packs addon
        '''
        try:
            self.logger.info(f'Packing {addon_id}')
            self.run(addon_id, ['-wbModule=ResourceManager', '-packAddon'])
        except:
            self.logger.critical(f'Packing {addon_id} failed')
            raise RuntimeError


    def publish_addon(self, addon_id, version, change_notes):
        '''
        Packs and publishes the addon with the given version and change notes
        '''
        try:
            self.logger.info(f'Publishing {addon_id}')
            self.run(addon_id, ['-wbModule=ResourceManager', '-packAddon', '-publishAddon', '-publishAddonVersion', version, '-publishAddonChangeNote', change_notes])
        except Exception as e:
            returncode = 1
            if hasattr(e, 'returncode'):
                returncode = e.returncode 
            
            if returncode == 3221225477:
                self.logger.error(f'Failed to publish {addon_id}, because {version} already exists')
            else:
                self.logger.critical(f'Publishing {addon_id} failed')
                raise RuntimeError


class CLI:
    '''
    Command line interface for make.py
    '''
    logger = logging.getLogger('Workbench')


    def __init__(self):
        self.parser = argparse.ArgumentParser(description='Tool to validate, pack, and publish Arma Reforger addons', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
        self.parser.add_argument('--branch', '-b', type=ReforgerBranch, action=ParseEnum, default=ReforgerBranch.STABLE, help='Select Reforger branch')
        self.parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose output')
        self.parser.add_argument('--just', '-j', help='Only processes the passed addon')
        self.subparsers = self.parser.add_subparsers(dest='command', required=True)
        self.subparsers.add_parser('validate', **self._gen_subparser_kwargs('Checks if scripts are compilable'))
        self.subparsers.add_parser('pack', **self._gen_subparser_kwargs('Packs the project'))
        self.subparsers.add_parser('publish', **self._gen_subparser_kwargs('Packs and publishes updates to the workshop'))


    def run(self, args=None):
        '''
        Parses and executes the command
        '''
        args = self.parser.parse_args(args)
        self.workbench = Workbench.from_steam(branch=args.branch, addons_dirs=PROJECT_ADDONS_DIRS, extra_args=WORKBENCH_EXTRA_ARGS)

        if args.verbose:
            logging.getLogger().setLevel(logging.DEBUG)

        method = getattr(self, args.command)
        method(args)


    def _gen_subparser_kwargs(self, help=None):
        '''
        Helper method for generating kwargs for subparser
        '''
        return {'help': help, 'description': help, 'formatter_class': self.parser.formatter_class}


    def _get_addons_to_process(self, args):
        '''
        Returns specific addon if --just is passed or all otherwise
        '''
        addons = get_all_addons()

        if args.just is not None:
            if args.just not in addons:
                self.logger.critical(f'Failed to locate "{args.just}"')
                raise RuntimeError

            return [args.just]
        else:
            return addons


    def validate(self, args):
        '''
        validate command
        '''
        self.logger.info('Start validating scripts...')

        for addon in self._get_addons_to_process(args):
            self.workbench.validate_scripts(addon)
        
        self.logger.info('Validation completed')


    def pack(self, args):
        '''
        pack command
        '''
        self.logger.info('Start packing...')

        for addon in self._get_addons_to_process(args):
            self.workbench.pack_addon(addon)

        self.logger.info('Packing completed')


    def publish(self, args):
        '''
        publish command
        '''
        tag = get_latest_tag()
        version = tag_to_version(tag)
        change_notes = get_release_url(tag)
        self.logger.info(f'Start publishing {tag}...')

        for addon in self._get_addons_to_process(args):
            self.workbench.publish_addon(addon, version, change_notes)

        self.logger.info('Publishing completed')


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format=LOGGER_FORMAT)
    cli = CLI()
    cli.run()

