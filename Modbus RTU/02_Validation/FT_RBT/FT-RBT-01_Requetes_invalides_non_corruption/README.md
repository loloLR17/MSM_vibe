# FT-RBT-01 — Requêtes invalides et non-corruption

## 1. Objet

FT-RBT-01 valide la robustesse d'un échange nominal lorsqu'une requête Modbus invalide est injectée entre deux échanges valides.

La sous-famille ne possède pas la définition de l'accès invalide ni l'exception Modbus associée : ces oracles restent propriétaires FT-ACC.

## 2. Périmètre actif

La sous-famille couvre uniquement la composition suivante :
- établir un état de référence observable ;
- injecter une requête invalide déjà qualifiée comme telle par FT-ACC ;
- vérifier l'absence de corruption observable ;
- vérifier qu'un échange valide ultérieur reste évalué selon son oracle nominal, sans imposer de délai de récupération.

## 3. Hors périmètre

Sont exclus :
- choix du code d'exception Modbus : FT-ACC ;
- classification RO/RW et adresses hors plage : FT-ACC ;
- valeurs métier hors domaine : FT-LIM / famille métier ;
- rafales de requêtes invalides et seuil de tolérance : `NOT_DEFINED` en V1 ;
- CRC de trame Modbus, trames tronquées ou framing : FT-RBT-05 / `NOT_DEFINED` tant qu'aucun oracle V1 n'existe ;
- timeout et temps de reprise : `NOT_DEFINED` ;
- reboot ou watchdog après erreur : FT-PER si spécifié.

## 4. Cas actif

- `TT-RBT-GEN-001` — requête invalide intercalée entre échanges valides : absence de corruption et maintien de l'applicabilité des oracles nominaux.

## 5. Limite importante

Le test ne mesure aucun « temps de récupération ». Il ne demande pas davantage une réponse particulière à une trame physiquement corrompue. Il utilise seulement une requête Modbus invalidée par les règles V1 d'accès.

## 6. Artefacts

- `source/FT-RBT-01_source.md` ;
- `detaille/FT-RBT-01_detaille.md` ;
- `detaille/FT-RBT-01_matrice_couverture.csv`.

## 7. Statut

Sous-famille reconstruite pour revue. Gel interdit avant validation explicite.
