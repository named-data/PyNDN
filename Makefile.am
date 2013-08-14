ACLOCAL_AMFLAGS = -I build-aux -I m4

SUBDIRS = csrc python . tests

checkdir = $(abs_top_builddir)/tests/install
tmpdir = $(abs_top_builddir)/tests/tmp

check-local:
	-mkdir $(tmpdir)
	$(MAKE) $(AM_MAKEFLAGS) install-exec DESTDIR=$(checkdir)

clean-local:
	-rm -rf $(checkdir)
	-rm -rf $(tmpdir)
