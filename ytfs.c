#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>

typedef struct YTFile {
	File *file;
	void (*read)(Req *);
	void *aux;
	char manual;
} YTFile;

YTFile *
createytfile(File *parent, char *name, char *user, ulong mode, void (*read)(Req *), void *aux) {
	YTFile *ytfile = malloc(sizeof(YTFile));
	File *file = createfile(parent, name, user, mode, ytfile);
	if (file == nil) {
		free(ytfile);
		return nil;
	}

	ytfile->file = file;
	ytfile->read = read;
	ytfile->aux = aux;

	return ytfile;
}

void
freeytfile(File *file) {
	YTFile *ytfile = (YTFile *)file->aux;
	if ((ytfile != nil) && !ytfile->manual) {
		free(file->aux);
	}
}

void
ctlread(Req *r) {
	readstr(r, "Not implemented.");
	respond(r, nil);
}

YTFile ctl = {
	.read = ctlread,
	.manual = 1,
};

void
fsinit(Srv *s) {
	char *user = getuser();

	s->tree = alloctree(user, user, 0555, freeytfile);
	if (s->tree == nil) {
		sysfatal("fsinit: alloctree: %r");
	}

	File *root = createfile(s->tree->root, "yt", user, DMDIR | 0555, nil);
	if (root == nil) {
		sysfatal("fsinit: createfile: %r");
	}
	
	ctl.file = createfile(root, "ctl", user, 0444, &ctl);
	if (ctl.file == nil) {
		sysfatal("fsinit: createfile: %r");
	}
}

void
fsread(Req *r) {
	YTFile *ytfile = (YTFile *)r->fid->file->aux;
	ytfile->read(r);
}

void
fsend(Srv *s) {
	sysfatal("fsend");
}

Srv s = {
	.read = fsread,
	.end = fsend,
};

void
threadmain(int argc, char *argv[]) {
	char *mtpt = "/mnt";
	char *srvname = "yt";

	ARGBEGIN {
	case 'm':
		mtpt = ARGF();
		break;

	case 's':
		srvname = ARGF();
		break;

	default:
		fprint(2, "usage: ytfs [-m mtpt] [-s srvname]\n");
		threadexits("usage");
	} ARGEND

	fsinit(&s);
	threadpostmountsrv(&s, srvname, mtpt, MBEFORE);
	threadexits(nil);
}
