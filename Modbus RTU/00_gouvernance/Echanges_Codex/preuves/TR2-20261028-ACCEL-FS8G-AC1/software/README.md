# Checkpoint logiciel AC1 / ±8g

Aucun accès flash/debug ni mutationpersistante surcible à cecheckpoint. Firmwarec36 et serviceWindows initial conservés. Sourcescore72a7b573 ; addendum36fd068 relu, extension8000–8063 explicitementautoriséepardécisionutilisateur. Ne pas présenter ces tests comme qualificationphysique.

148testsC et cross-buildcanonique PASS (`full-validation-bash.log`), imageopérateurbuild PASS. ASan/UBSan :3suitesPASS, dont448oraclesB3inchangés ettransfertAC1. Erreursinitiales conservées : typeCconfigcorrigé, fixtureSTARTtest àtempsrégressif corrigé, mauvaiscalibredufixturewire corrigé, champI32dufixture.NET corrigé au−950000µgattendu. Aucune attenteaffaiblie ; retestsciblésdansfichiersfinaux. Premierwiremodel arrêté car sonhorlogesimulée n’avançaitpas ; bouclepilotéeensuiteparlecheckpointcoopératifauthentique.

.NET :305testsautressuitesPASS ; protocole52PASSfinaletService252PASSfinal (exécutionsséparées, premieréchecprotocolfixtureconservé). JS :5nouveauxAC1 etrégressionshistoriques. `BUNDLE.json` donne empreintesbrutes/stockées ; `.zlib` préservelasortieeffective. PaquetWindows/gate, acquisitionsphysiques, applicationB4durable±8g, resetetpreuvesEdge/SQLite restentàeffectuer danslasuitemission.
