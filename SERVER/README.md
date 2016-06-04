Server
======


Features
--------

* End goal: provide data from a 3rd party HTTP API to a 4x20 LCD display.
* Data flow: the main microcontroller will poll another network
  microcontroller, which will poll this HTTP server for 4x20 ASCII characters.
  The main microcontroller will push that text to the LCD display.
* Cache: This HTTP server will regularly fetch the latest data from a 3rd party
  HTTP API at a fixed interval. The response will be cached. This cache will
  control and limit the number of requests to the 3rd party API.


Work Breakdown
--------------

* Getting data from 3rd party API (Trafiklab SL API data provider)
* Scheduling (cron-like 3rd party API fetching)
* Cache (store the scheduled fetched data)
* Serving HTTP (serving contents of cache)


Dev Ideas
---------

* Walking skeleton tests can be written in Python, using built-in `unittest` as
  test runner.
* There's a separate "App" project that combines the data provider and makes
  the whole thing serve HTTP and implement its cron-like bahavior.
* Would be fun to write the server in Clojure.
* Packaged as a `.deb` using FPM, complete with init scripts (systemd).
* System tested using Vagrant and a vanilla Debian 8 box.


Requirements
------------

* Python
* Vagrant: https://www.vagrantup.com
* vagrant-vbguest: https://github.com/dotless-de/vagrant-vbguest
* 
