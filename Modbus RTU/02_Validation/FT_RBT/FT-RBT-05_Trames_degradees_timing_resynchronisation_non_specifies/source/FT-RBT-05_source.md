# FT-RBT-05 — Exigences source normalisées

## 1. Références examinées

- `01_Specification_source/charte_typage.md` V1 ;
- blocs applicatifs V1 ;
- familles gelées FT-ACC, FT-CMD, FT-SEQ ;
- artefacts FT-RBT-01 à FT-RBT-04.

## 2. Exigences / dettes

### RBT05-R01 — Trame RTU avec CRC de trame invalide
- Classification : `NOT_DEFINED`.
- Justification : aucun comportement TR2 V1 explicite n'est défini pour une trame reçue avec CRC Modbus RTU invalide.
- Aucun verdict ne doit imposer silence, exception, compteur diagnostic ou autre réaction sans source normative explicite.

### RBT05-R02 — Trame tronquée / framing invalide
- Classification : `NOT_DEFINED`.
- Justification : aucune règle V1 ne fixe la réaction applicative attendue, la purge du buffer ou le mécanisme de récupération.

### RBT05-R03 — Résynchronisation après trame dégradée
- Classification : `NOT_DEFINED`.
- Justification : aucun délai, nombre de trames, séquence de reprise ou événement de resynchronisation n'est spécifié.

### RBT05-R04 — Timeout de réponse Modbus
- Classification : `NOT_DEFINED`.
- Justification : aucune borne de temps de réponse protocolaire n'est normative dans la V1.

### RBT05-R05 — Politique de retry / backoff
- Classification : `NOT_DEFINED`.
- Justification : aucun nombre de retries, délai entre tentatives, backoff ou condition d'abandon n'est défini.

### RBT05-R06 — Fréquence maximale / débit minimal
- Classification : `NOT_DEFINED`.
- Justification : aucune exigence quantitative de cadence de requêtes ou de débit transactionnel n'est définie.

### RBT05-R07 — Rafale de trames invalides / récupération automatique
- Classification : `NOT_DEFINED`.
- Justification : aucun nombre de fautes consécutives, seuil de robustesse, watchdog, reset ou délai de récupération n'est défini.

### RBT05-R08 — Priorité entre erreurs simultanées de transport
- Classification : `NOT_DEFINED`.
- Justification : aucune hiérarchie de traitement n'est définie lorsqu'une trame présente plusieurs défauts simultanés.

### RBT05-R09 — Accès invalide au sens mapping
- Classification : `DELEGATED`.
- Propriétaire : FT-ACC / charte de typage §14.
- Justification : ce cas possède déjà un oracle explicite et ne doit pas être confondu avec une trame RTU corrompue.

### RBT05-R10 — CRC applicatif / configuration
- Classification : `DELEGATED`.
- Propriétaire : famille fonctionnelle du bloc concerné.
- Justification : un CRC métier protège une structure applicative et ne définit pas le traitement d'une trame Modbus RTU erronée.

## 3. Anti-fabrication

Ne pas importer implicitement comme exigences TR2 :
- silence obligatoire sur CRC incorrect ;
- code exception particulier ;
- temps inter-trames ou timeout spécifique ;
- purge automatique après N octets ;
- récupération après N trames correctes ;
- compteur d'erreurs obligatoire ;
- reboot ou watchdog après défaut bus.

## 4. Conclusion

FT-RBT-05 ne crée aucun test PASS/FAIL autonome dans la V1 actuelle. Elle ferme explicitement les zones non normées pour empêcher qu'une future campagne d'essais attribue à tort au TR2 des exigences qui n'ont jamais été spécifiées.
