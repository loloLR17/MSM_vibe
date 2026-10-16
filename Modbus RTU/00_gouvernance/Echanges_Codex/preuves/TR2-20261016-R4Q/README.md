# R4-Q — preuves

Les horloges FIFO/modèle et allowances CPU des tests sont SIMULÉES. Les mesures
STM32 sont séparées dans hardware/*.log/json : RTU loopback RAM, jamais RS485
ni mesure vibratoire B3 physique. Le rapport précise les phases effectivement
exécutées. Les erreurs de construction des nouvelles fixtures restent conservées.
Snapshots .bin.zlib sans perte : zlib.decompress(),262144octets FRAM.
Backups Flash2Mio récupérables hors Git sous reports/TR2-r4q/hardware et anciens
backups R4P ; empreintes publiées. Probe NON-PRODUCTION, pas firmware opérationnel.
Logs Windows CRLF conservés intégraux. SHA256SUMS couvre tout sauf lui-même.
