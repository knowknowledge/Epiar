Name:           epiar
Version:        0.5.0
Release:        1%{?dist}
Summary:        A space trading/exploring arcade game

Group:          Amusements/Games
License:        GPLv2+
URL:            http://epiar.net/
Source0:        http://epiar.net/files/epiar/releases/0.5.0/%{name}-%{version}.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:	SDL-devel
BuildRequires:	SDL_image-devel
BuildRequires:	SDL_mixer-devel
BuildRequires:	libxml2-devel
BuildRequires:	ftgl-devel
BuildRequires:	physfs-devel

%description
Epiar (ep-ee-are) is a space trading/exploring arcade simulation game
in which the player navigates space from planet to planet, saving
money to buy ship upgrades and new ships. The player can also join
mercenary missions, attack other ships to steal their money and
technology, and explore the universe.


%prep
%setup -q -n %{name}-%{version}

%build
%configure
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

desktop-file-install --vendor epiar --delete-original	\
	--dir $RPM_BUILD_ROOT%{_datadir}/applications	\
	%{_builddir}/%{name}-%{version}/%{name}.desktop

%clean
rm -rf $RPM_BUILD_ROOT

%post

%preun

%postun


%files
%defattr(-,root,root)
%doc COPYING NEWS README
%{_bindir}/epiar
%{_datadir}/%{name}/
%{_datadir}/applications/epiar.desktop
%{_datadir}/pixmaps/epiar.png

%changelog
* Mon May 16 2011 Christopher Thielen <chris@epiar.net> 0.5.0
- Initial creation using Epiar version 0.5.0.

