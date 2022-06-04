import datetime
import re
import subprocess
import sys

VERSION_FILE = "include/version.h"

class Version:
    def __init__(self):
        self.major = None
        self.minor = None
        self.release = False
        self.release_ver = None
        self.release_msg = None

        # @property head
        # @property commit
        # @property build
        # @property date
        # @property ver_date
        # @property head_tags
        self._head = None
        self._commit = None
        self._date = None
        self._ver_date = None
        self._head_tags = None

    def load(self, rel=False, ver=None, msg=None):
        """load version attributes"""
        self.release = rel
        self.release_ver = ver
        self.release_msg = msg
        if self.release:
            print("Release")
            # prepare release tag
            if self.release_ver is not None:
                self.from_release_ver() # from provided release version
            else:
                # firts, try existing release tag
                if not self.from_release_tag():
                    # else prepare version based on commit date
                    self.from_date()
        else:
            print("Build")
            # make version based on information from repository
            # first, try to get version from release tag
            if not self.from_release_tag():
                # then get automatic version based on commit date
                self.from_date()

    def get_major(self):
        if self.major:
            return self.major
        # no major, provide something based on (current) date
        sl = self.date.split('-')
        return "{}{}".format(sl[0][-2:], sl[1])

    def get_minor(self):
        return self.minor or "0"

    def get_build(self):
        return self.build or "NOGIT"

    def get_full(self):
        return "{}.{}.{}".format(self.get_major(), self.get_minor(), self.get_build())

    def get_tag(self):
        if self.release_ver:
            return "release-{}".format(self.release_ver)
        else:
            return "release-{}.{}".format(self.get_major(), self.get_minor())

    def from_release_ver(self):
        """load version from provided release version"""
        try:
            self.major, self.minor = self.release_ver.split('.', 1)
        except ValueError:
            self.major, self.minor = self.release_ver, ""

    def from_date(self):
        """load version from date of HEAD commit"""
        # major
        self.major = self.ver_date
        # minor
        if self.release:
            # get proper minor for release version
            release_tags = []
            # get all tags matching release-{yymm}.* pattern
            if self.ver_date:
                try:
                    release_tags = subprocess.check_output(
                        ["git", "tag", "-l", "release-{}.*".format(self.ver_date)], universal_newlines=True).splitlines()
                except subprocess.CalledProcessError:
                    pass
                print("Matching tags:", release_tags)
                # find last used ver_min
                max_min = 0
                for tag in release_tags:
                    try:
                        n = int(tag.split(".")[1])
                        if max_min <= n:
                            max_min = n
                    except ValueError:
                        pass
                self.minor = "{}".format(max_min+1)
            else:
                self.minor = ""
        else:
            # not a release, use 0 for minor
            self.minor = "0"

    def from_release_tag(self):
        """load version from release tag on HEAD"""
        release_tags = [tag for tag in self.head_tags if tag.lower().startswith("release-")]
        release_tags.sort(reverse=True)
        if not release_tags:
            return False
        # get version from release tag
        release_version = release_tags[0].split('-', 1)[1]
        try:
            self.major, self.minor = release_version.split('.', 1)
        except ValueError:
            self.major, self.minor =  release_version, ""
        print("version from tag:", release_version, "->", self.major, self.minor)
        return True

    @property
    def build(self):
        return self.commit

    @property
    def commit(self):
        """return short version of commit hash"""
        if self._commit is None:
            try:
                self._commit = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], universal_newlines=True).strip()
            except subprocess.CalledProcessError:
                self._commit = ""
            print("Commit:", self._commit)
        return self._commit

    @property
    def head(self):
        """return commit hash"""
        if self._head is None:
            try:
                self._head = subprocess.check_output(["git", "rev-parse", "HEAD"], universal_newlines=True).strip()
            except subprocess.CalledProcessError:
                self._head = ""
            print("HEAD:", self._head)
        return self._head

    @property
    def date(self):
        """return HEAD commit date"""
        if self._date is None:
            self._date = datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S")
            if self.head:
                try:
                    self._date = subprocess.check_output(
                        ["git", "show", "--quiet", "--date=format-local:%Y-%m-%d %H:%M:%S", "--format=%cd", self.head],
                        env={'TZ': 'UTC0'}, universal_newlines=True).strip()
                    # print("HEAD date:", self._date)
                except subprocess.CalledProcessError:
                    pass
        return self._date

    @property
    def ver_date(self):
        """return date part of version: yymm"""
        if self._ver_date is None:
            self._ver_date = ""
            if self.head:
                try:
                    self._ver_date = subprocess.check_output(
                        ["git", "show", "--quiet", "--date=format-local:%y%m", "--format=%cd", self.head],
                        env={'TZ': 'UTC0'}, universal_newlines=True).strip()
                    # print("HEAD ver date:", self._ver_date)
                except subprocess.CalledProcessError:
                    pass
            # print("ver date:", self._ver_date)
        return self._ver_date

    @property
    def head_tags(self):
        """return list of tags pointing to HEAD"""
        if self._head_tags is None:
            try:
                self._head_tags = subprocess.check_output(["git", "tag", "--points-at", "HEAD"], universal_newlines=True).splitlines()
            except subprocess.CalledProcessError:
                self._head_tags = []
            print("HEAD tags:", self._head_tags)
        return self._head_tags


def create_release_tag(ver, push):
    tag = ver.get_tag()
    print("Release tag:", tag)
    if tag in ver.head_tags:
        print("Release tag alredy on HEAD")
        return 0
    try:
        print("Adding tag")
        out = subprocess.check_output(["git", "tag", tag], universal_newlines=True).splitlines()
    except subprocess.CalledProcessError:
        print("Failed to add tag:", tag)
        return -1
    if push:
        print("Pushing tag to origin")
        try:
            out = subprocess.check_output(["git", "push", "origin", tag], universal_newlines=True).splitlines()
        except subprocess.CalledProcessError:
            print("Failed to push tag")
            return -1
    print("Done")
    return 0


def update_version_file(ver):
    defs = [
        ('FN_VERSION_MAJOR', ver.get_major()),
        ('FN_VERSION_MINOR', ver.get_minor()),
        ('FN_VERSION_BUILD', ver.get_build()),
        ('FN_VERSION_DATE', ver.date),
        ('FN_VERSION_FULL', ver.get_full())
    ]

    lines = open(VERSION_FILE).readlines()
    lines_new = []

    for line in lines:
        for df, val in defs:
            start = "#define {} ".format(df)
            if line.startswith(start):
                line = '{}"{}"\n'.format(start, val)
                break
        lines_new.append(line)

    # update file only if necessary
    if lines != lines_new:
        print("Updating", VERSION_FILE)
        with open(VERSION_FILE, "w") as fout:
            fout.writelines(lines_new)
    else:
        print("Version file is up to date")
    return 0


def main():

    # # Don't do anything if nothing has changed
    # if len(subprocess.check_output(["git", "diff", "--name-only"], universal_newlines=True)) == 0:
    #     print("Nothing has changed")
    #     return 0

    release = False
    push = False
    rel_ver = None
    rel_msg = None

    # arguments
    if len(sys.argv) > 1 and (sys.argv[1] == "release" or sys.argv[1].startswith("release:")):
        release = True
        if sys.argv[1].startswith("release:") and sys.argv[1][8:] == "push":
            push = True
        if len(sys.argv) > 2:
            rel_ver = sys.argv[2]
            if len(sys.argv) > 3:
                rel_msg = sys.argv[3]

    ver = Version()
    ver.load(release, rel_ver, rel_msg)

    # print("date:", ver.date)
    print("Build version:", ver.get_full())
    print("Version date:", ver.date)

    if release:
        return(create_release_tag(ver, push))

    return(update_version_file(ver))


if __name__ == '__main__':
    sys.exit(main() or 0)
