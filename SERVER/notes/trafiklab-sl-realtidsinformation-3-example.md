# Documentation

https://www.trafiklab.se/api/sl-realtidsinformation-3/sl-realtidsinformation-3


# Example

    $ http GET http://api.sl.se/api2/realtimedepartures.json key==deadbeef siteid==9201 timewindow==10
    HTTP/1.1 200 OK
    Cache-Control: no-cache
    Content-Length: 6710
    Content-Type: application/json; charset=utf-8
    Date: Thu, 24 Mar 2016 13:36:30 GMT
    Expires: -1
    Pragma: no-cache
    Server: Microsoft-IIS/8.5
    X-Powered-By: ASP.NET
    server: Microsoft-IIS/8.5
    x-aspnet-version: 4.0.30319
    x-powered-by: ASP.NET

    {
        "ExecutionTime": 463,
        "Message": null,
        "ResponseData": {
            "Buses": [
                {
                    "Destination": "Vaxholm",
                    "Deviations": null,
                    "DisplayTime": "Nu",
                    "ExpectedDateTime": "2016-03-24T14:37:00",
                    "GroupOfLine": "blåbuss",
                    "JourneyDirection": 1,
                    "LineNumber": "670",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "F",
                    "StopPointNumber": 60001,
                    "TimeTabledDateTime": "2016-03-24T14:37:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Danderyds sjh via Sätra äng",
                    "Deviations": null,
                    "DisplayTime": "14:37",
                    "ExpectedDateTime": "2016-03-24T14:37:00",
                    "GroupOfLine": null,
                    "JourneyDirection": 2,
                    "LineNumber": "602H",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "L",
                    "StopPointNumber": 60005,
                    "TimeTabledDateTime": "2016-03-24T14:37:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Jakobsbergs station",
                    "Deviations": null,
                    "DisplayTime": "1 min",
                    "ExpectedDateTime": "2016-03-24T14:38:00",
                    "GroupOfLine": "blåbuss",
                    "JourneyDirection": 1,
                    "LineNumber": "178",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "B",
                    "StopPointNumber": 60016,
                    "TimeTabledDateTime": "2016-03-24T14:38:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Karolinska sjh via Sollentuna",
                    "Deviations": null,
                    "DisplayTime": "14:39",
                    "ExpectedDateTime": "2016-03-24T14:39:00",
                    "GroupOfLine": null,
                    "JourneyDirection": 1,
                    "LineNumber": "607",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "N",
                    "StopPointNumber": 60015,
                    "TimeTabledDateTime": "2016-03-24T14:39:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Linanäs",
                    "Deviations": null,
                    "DisplayTime": "14:40",
                    "ExpectedDateTime": "2016-03-24T14:40:00",
                    "GroupOfLine": null,
                    "JourneyDirection": 1,
                    "LineNumber": "626",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "O",
                    "StopPointNumber": 60007,
                    "TimeTabledDateTime": "2016-03-24T14:40:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Mörby station",
                    "Deviations": null,
                    "DisplayTime": "3 min",
                    "ExpectedDateTime": "2016-03-24T14:40:23",
                    "GroupOfLine": "blåbuss",
                    "JourneyDirection": 2,
                    "LineNumber": "178",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "H",
                    "StopPointNumber": 60008,
                    "TimeTabledDateTime": "2016-03-24T14:34:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Norrtälje",
                    "Deviations": null,
                    "DisplayTime": "4 min",
                    "ExpectedDateTime": "2016-03-24T14:41:00",
                    "GroupOfLine": "blåbuss",
                    "JourneyDirection": 1,
                    "LineNumber": "676",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "F",
                    "StopPointNumber": 60001,
                    "TimeTabledDateTime": "2016-03-24T14:41:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Skärvik",
                    "Deviations": null,
                    "DisplayTime": "14:41",
                    "ExpectedDateTime": "2016-03-24T14:41:00",
                    "GroupOfLine": "blåbuss",
                    "JourneyDirection": 2,
                    "LineNumber": "177",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "A",
                    "StopPointNumber": 50366,
                    "TimeTabledDateTime": "2016-03-24T14:41:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Vallentuna (Rosengården)",
                    "Deviations": null,
                    "DisplayTime": "14:42",
                    "ExpectedDateTime": "2016-03-24T14:42:00",
                    "GroupOfLine": null,
                    "JourneyDirection": 1,
                    "LineNumber": "610",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "G",
                    "StopPointNumber": 60009,
                    "TimeTabledDateTime": "2016-03-24T14:42:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Danderyds sjh via Stocksund",
                    "Deviations": null,
                    "DisplayTime": "6 min",
                    "ExpectedDateTime": "2016-03-24T14:43:04",
                    "GroupOfLine": null,
                    "JourneyDirection": 1,
                    "LineNumber": "601V",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "L",
                    "StopPointNumber": 60005,
                    "TimeTabledDateTime": "2016-03-24T14:41:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Stockholm",
                    "Deviations": null,
                    "DisplayTime": "7 min",
                    "ExpectedDateTime": "2016-03-24T14:43:50",
                    "GroupOfLine": "blåbuss",
                    "JourneyDirection": 2,
                    "LineNumber": "676",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "C",
                    "StopPointNumber": 60010,
                    "TimeTabledDateTime": "2016-03-24T14:42:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Mörby station",
                    "Deviations": null,
                    "DisplayTime": "7 min",
                    "ExpectedDateTime": "2016-03-24T14:44:28",
                    "GroupOfLine": "blåbuss",
                    "JourneyDirection": 1,
                    "LineNumber": "176",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "H",
                    "StopPointNumber": 60008,
                    "TimeTabledDateTime": "2016-03-24T14:35:51",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Erikslund",
                    "Deviations": null,
                    "DisplayTime": "14:45",
                    "ExpectedDateTime": "2016-03-24T14:45:00",
                    "GroupOfLine": null,
                    "JourneyDirection": 1,
                    "LineNumber": "611",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "J",
                    "StopPointNumber": 60003,
                    "TimeTabledDateTime": "2016-03-24T14:45:00",
                    "TransportMode": "BUS"
                },
                {
                    "Destination": "Danderyds sjukhus",
                    "Deviations": null,
                    "DisplayTime": "9 min",
                    "ExpectedDateTime": "2016-03-24T14:45:36",
                    "GroupOfLine": null,
                    "JourneyDirection": 2,
                    "LineNumber": "601H",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "StopAreaNumber": 50366,
                    "StopPointDesignation": "H",
                    "StopPointNumber": 60008,
                    "TimeTabledDateTime": "2016-03-24T14:40:00",
                    "TransportMode": "BUS"
                }
            ],
            "DataAge": 48,
            "LatestUpdate": "2016-03-24T14:35:42",
            "Metros": [
                {
                    "DepartureGroupId": 1,
                    "Destination": "Mörby centrum",
                    "DisplayTime": "6 min",
                    "GroupOfLine": "Tunnelbanans röda linje",
                    "GroupOfLineId": 2,
                    "JourneyDirection": 1,
                    "LineNumber": "14",
                    "PlatformMessage": null,
                    "SafeDestinationName": "Mörby centrum",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "TransportMode": "METRO"
                },
                {
                    "DepartureGroupId": 1,
                    "Destination": "Mörby centrum",
                    "DisplayTime": "11 min",
                    "GroupOfLine": "Tunnelbanans röda linje",
                    "GroupOfLineId": 2,
                    "JourneyDirection": 1,
                    "LineNumber": "14",
                    "PlatformMessage": null,
                    "SafeDestinationName": "Mörby centrum",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "TransportMode": "METRO"
                },
                {
                    "DepartureGroupId": 1,
                    "Destination": "Mörby centrum",
                    "DisplayTime": "17 min",
                    "GroupOfLine": "Tunnelbanans röda linje",
                    "GroupOfLineId": 2,
                    "JourneyDirection": 1,
                    "LineNumber": "14",
                    "PlatformMessage": null,
                    "SafeDestinationName": "Mörby centrum",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "TransportMode": "METRO"
                },
                {
                    "DepartureGroupId": 2,
                    "Destination": "Fruängen",
                    "DisplayTime": "Nu",
                    "GroupOfLine": "Tunnelbanans röda linje",
                    "GroupOfLineId": 2,
                    "JourneyDirection": 2,
                    "LineNumber": "14",
                    "PlatformMessage": "Pendeltågen: Ingen trafik mellan Centralen och Älvsjö från 24/3 kl 22 t.o.m. 28/3 p.g.a. banarbeten. Tågen ersätts med bussar mellan Telefonplan och Älvsjö.",
                    "SafeDestinationName": "Fruängen",
                    "SiteId": 9201,
                    "StopAreaName": "Danderyds sjukhus",
                    "TransportMode": "METRO"
                }
            ],
            "Ships": [],
            "StopPointDeviations": [],
            "Trains": [],
            "Trams": []
        },
        "StatusCode": 0
    }
