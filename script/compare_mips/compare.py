import argparse
import json
import pathlib
import shutil

from mako.template import Template

ISSUE_TEMPLATE = r'''**Status** (for commit ${current_hash})**:** ${message}

**Current dhrystone MIPS** (in commit ${current_hash})**:** ${new_mips}
**Previous best** (recorded in commit ${best_hash})**:** ${best_mips}, difference ${f'{best_diff:+.2%}'}

<sub>This comment was created automatically, please do not change!</sub>
'''

def main(new_file, old_file, current_hash, tolerance, no_update):
    issue_template = Template(text=ISSUE_TEMPLATE)

    new_path = pathlib.Path(new_file)
    old_path = pathlib.Path(old_file)

    if not new_path.exists(): #checks whether new path exists or not
        raise ValueError('file to compare does not exist!')

    if not old_path.exists():
        print('WARN: file to compare against does not exist, assuming first compare')
        shutil.copy(new_path, old_path)

    with open(new_path, 'r') as f1, open(old_path, 'r') as f2:
        new_dict = json.load(f1)
        old_dict = json.load(f2)

    new_mips = new_dict['mips']

    old_best_mips = best_mips = old_dict.get('best_mips', 0.00000001)
    old_best_hash = best_hash = old_dict.get('best_hash', None)
    regressed_hash = old_dict.get('regressed_hash', None)

    best_diff = new_mips / best_mips - 1

    regressed = False

    if best_diff < -tolerance:
        message = f'⚠ Major regression since commit {regressed_hash} ⚠'
        print('major regression')
        if regressed_hash is None:
            message = f'⚠ Major regression introduced! ⚠'
            regressed_hash = current_hash
        regressed = True

    elif new_mips > best_mips:
        print('new best')
        message = '🥇 New best performance!'
        best_mips = new_mips
        best_hash = current_hash
        regressed_hash = None

    else:
        if regressed_hash is not None:
            message = 'Regression cleared'
            print('regression cleared')
        else:
            message = 'No significant performance change'
            print('no significant change')
        regressed_hash = None

    new_dict['best_mips'] = best_mips
    new_dict['best_hash'] = best_hash
    new_dict['regressed_hash'] = regressed_hash

    if not no_update:
        with open(new_path, 'w') as f1:
            json.dump(new_dict, f1)

    with open('mips_issue_text.md', 'w') as f1:
        f1.write(issue_template.render(
            current_hash=current_hash,
            new_mips=new_mips,
            message=message,
            best_mips=old_best_mips,
            best_hash=old_best_hash,
            best_diff=best_diff
        ))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('new_file')
    parser.add_argument('old_file')
    parser.add_argument('git_commit_hash')
    parser.add_argument('-t', '--tolerance', default=0.2)
    parser.add_argument('-n', '--no_update', action='store_true')

    args = parser.parse_args()

    main(args.new_file, args.old_file, args.git_commit_hash, args.tolerance, args.no_update)