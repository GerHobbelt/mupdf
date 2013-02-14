Name:           mupdf
Version:        1.1
Release:        3%{?dist}
Summary:        A lightweight PDF viewer and toolkit
Group:          Applications/Publishing
License:        GPLv3
URL:            http://mupdf.com/
Source0:        http://mupdf.com/download/%{name}-%{version}-source.tar.gz
Source1:        %{name}.desktop
## http://bugs.ghostscript.com/show_bug.cgi?format=multiple&id=693010
Patch0:         %{name}-upstream.patch
BuildRequires:  openjpeg-devel jbig2dec-devel desktop-file-utils
BuildRequires:  libjpeg-devel freetype-devel libXext-devel

%description
MuPDF is a lightweight PDF viewer and toolkit written in portable C.
The renderer in MuPDF is tailored for high quality anti-aliased
graphics.  MuPDF renders text with metrics and spacing accurate to
within fractions of a pixel for the highest fidelity in reproducing
the look of a printed page on screen.
MuPDF has a small footprint.  A binary that includes the standard
Roman fonts is only one megabyte.  A build with full CJK support
(including an Asian font) is approximately five megabytes.
MuPDF has support for all non-interactive PDF 1.7 features, and the
toolkit provides a simple API for accessing the internal structures of
the PDF document.  Example code for navigating interactive links and
bookmarks, encrypting PDF files, extracting fonts, images, and
searchable text, and rendering pages to image files is provided.

%package devel
Summary:        Development files for %{name}
Group:            Development/Libraries
Requires:         %{name} = %{version}-%{release}
Provides:         %{name}-static = %{version}-%{release}

%description devel
The mupdf-devel package contains header files for developing
applications that use mupdf and static libraries

%prep
%setup -q -n %{name}-%{version}-source
%patch0 -p1 

%build
export CFLAGS="%{optflags}"
make  -j1 
## %{?_smp_mflags} verbose=1 

%install
make DESTDIR=%{buildroot} install prefix=%{buildroot}/usr libdir=%{buildroot}%{_libdir}
desktop-file-install --dir=%{buildroot}%{_datadir}/applications %{SOURCE1}
install -D -m644 debian/%{name}.xpm %{buildroot}/%{_datadir}/pixmaps/%{name}.xpm
## filename conflict with poppler
### mv %{buildroot}%{_bindir}/pdfinfo %{buildroot}%{_bindir}/pdfinfo-mupdf
## fix strange permissons
chmod 0644 %{buildroot}/%{_includedir}/*.h
chmod 0644 %{buildroot}%{_libdir}/*.a
find %{buildroot}/%{_mandir} -type f -exec chmod 0644 {} \;

%post
update-desktop-database &> /dev/null || :

%postun
update-desktop-database &> /dev/null || :

%files
%defattr(-,root,root,-)
%doc COPYING README
%{_bindir}/mupdf
%{_bindir}/mudraw
%{_bindir}/mubusy
%{_datadir}/applications/mupdf.desktop
%{_mandir}/man1/mudraw.1.gz
%{_mandir}/man1/mubusy.1.gz
%{_mandir}/man1/mupdf.1.gz
%{_datadir}/pixmaps/mupdf.xpm



%files devel
%defattr(-,root,root,-)
%{_includedir}/fitz.h
%{_includedir}/memento.h
%{_includedir}/mucbz.h
%{_includedir}/mupdf.h
%{_includedir}/muxps.h
%{_libdir}/libfitz.a

%changelog
* Thu Feb 14 2013 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.1-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_19_Mass_Rebuild

* Mon Jan 21 2013 Adam Tkac <atkac redhat com> - 1.1-2
- rebuild due to "jpeg8-ABI" feature drop

* Wed Jan 09 2013 Pavel Zhukov <landgraf@fedoraproject.org> - 1.1-1
- New release

* Sun May 20 2012  Pavel Zhukov <landgraf@fedoraproject.org> - 1.0-1
- New release

* Wed Mar 14 2012  Pavel Zhukov <landgraf@fedoraproject.org> - 0.9-2
- Fix buffer overflow (#752388)

* Fri Jan 13 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 0.9-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Wed Oct 27 2011 Pavel Zhukov <landgraf@fedoraproject.org> - 0.9-1
- New release

* Tue May 03 2011 Pavel Zhukov <landgraf@fedoraproject.org> - 0.8.165-2
- New upstream release
- Fix *.a and *.h permissions

* Sun Mar 27 2011 Pavel Zhukov <landgraf@fedoraproject.org> - 0.8.15-1
- New upstream release

* Tue Feb 9 2011 Pavel Zhukov <landgraf@fedoraproject.org> - 0.7-7
- Fix dependency for F13

* Sun Feb 7 2011 Pavel Zhukov <landgraf@fedoraproject.org> - 0.7-6
- roll back to static libraries  patch for shared libs has been rejected
- Fix spec errors 

* Fri Jan 14 2011 Pavel Zhukov <landgraf@fedoraproject.org> - 0.7-4
- replac poitless macros to command names

* Fri Jan 14 2011 Pavel Zhukov <landgraf@fedoraproject.org> - 0.7-3
- Create patch for optflags
- Change Summary
- Fix Require for devel package

* Thu Jan 13 2011 Pavel Zhukov <landgraf@fedoraproject.org> -0.7-2
- add Fedora CFLAGS
- create patch for use shared library

* Wed Jan 12 2011 Pavel Zhukov <landgraf@fedoraproject.org>  - 0.7-1
- Initial package
