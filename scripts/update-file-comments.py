#!/usr/bin/env python

"""
Updates documentation comments at the top of C++ files to comply with
the CS3307 Coding Style Guidelines.

This script expects to be run as a pre-commit hook and will make some
decisions about authors and dates based on that assumption.
"""

import functools
import os
import re
import subprocess
import sys
import traceback
from datetime import date
from typing import Optional


def read_comment_from_file(file_path: str) -> Optional[str]:
    """
    Reads a C/C++ block comment from the start of a file, cleaning up
    any extra * characters in the content.
    """

    with open(file_path, "r") as file:
        file_content = file.read()

    if not file_content.startswith("/*"):
        return None

    comment_content, _, _ = file_content[2:].partition("*/")
    return "\n".join(
        line.lstrip("* ").rstrip() for line in comment_content.splitlines()
    ).strip()


def write_comment_to_file(file_path: str, content: str):
    """
    Writes the given text as a documentation comment in the given file.

    If a block comment already exists at the start of the file, this
    function will replace it.
    """

    with open(file_path, "r") as file:
        file_content = file.read()

    original_file_content = file_content
    if file_content.startswith("/*"):
        _, _, file_content = file_content.partition("*/")

    comment = "\n".join(
        ["/**", *(f" * {line}".rstrip() for line in content.splitlines()), " */\n\n"]
    )

    file_content = comment + file_content.lstrip()

    # If the content hasn't changed, don't bother rewriting the file.
    if file_content == original_file_content:
        return

    with open(file_path, "w") as file:
        file.write(file_content)


def get_git_authors(file_path: str) -> set[str]:
    """
    Determines the names of authors of a file according to Git.
    """
    authors = (
        subprocess.check_output(["git", "log", "--format=%aN", file_path])
        .decode()
        .split("\n")
    )
    return set(authors)


def get_file_last_committed_date(file_path: str) -> str:
    """
    Determines the date of the last commit containing a file according
    to Git.
    """
    return (
        subprocess.check_output(["git", "log", "--format=%as", "-1", file_path])
        .decode()
        .strip()
    )


def is_file_dirty(file_path: str) -> bool:
    """
    Checks if a file has uncommitted changes or is untracked.
    """
    return (
        subprocess.run(
            ["git", "diff-index", "--quiet", "HEAD", "--", file_path]
        ).returncode
        != 0
    )


@functools.cache
def get_git_username():
    """
    Determines the name of the current Git user.
    """
    username = subprocess.check_output(["git", "config", "user.name"]).decode().strip()
    return username


directive_regex = re.compile(r"^\\(?:class|authors|date) .*$", re.MULTILINE)


def update_comment(file_path: str, content: str) -> str:
    content_without_directives = re.sub(directive_regex, "", content).strip()

    class_name, _, _ = os.path.basename(file_path).rpartition(".")
    class_directive = f"\\class {class_name}\n\n" if class_name != "main" else ""

    authors = get_git_authors(file_path)
    if is_file_dirty(file_path):
        authors.add(get_git_username())
    authors.remove("")
    authors_list_str = ", ".join(sorted(authors))
    authors_directive = f"\\authors {authors_list_str}"

    if is_file_dirty(file_path):
        last_modified_date_str = str(date.today())
    else:
        last_modified_date_str = get_file_last_committed_date(file_path)
    date_directive = f"\\date {last_modified_date_str} (last updated)"

    new_content = (
        f"{class_directive}"  # class_directive contains \n characters
        f"{content_without_directives}\n\n"
        f"{authors_directive}\n"
        f"{date_directive}"
    )
    return new_content


def main():
    files_to_update = sys.argv[1:]
    if len(files_to_update) == 0:
        print("No files to update")
        sys.exit(0)

    exit_status = 0
    for file_path in files_to_update:
        try:
            comment = read_comment_from_file(file_path)
            if comment is None:
                raise RuntimeError(f"Missing comment in {file_path}")
            comment = update_comment(file_path, comment)
            write_comment_to_file(file_path, comment)
        except Exception:
            traceback.print_exc()
            exit_status = 1

    sys.exit(exit_status)


if __name__ == "__main__":
    main()
