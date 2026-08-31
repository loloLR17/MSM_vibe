# FT-RBT-05 — Trames dégradées, timing et résynchronisation non spécifiés

## 1. Objet

FT-RBT-05 formalise les limites normatives de la V1 concernant les défauts de trame Modbus RTU, le timing de communication et les mécanismes de résynchronisation.

## 2. Conclusion de périmètre

La V1 du projet TR2 ne définit pas de comportement vérifiable spécifique pour :
- trame Modbus RTU avec CRC de trame invalide ;
- trame tronquée ou framing invalide ;
- reprise après bruit ou séquence de trames invalides ;
- délai maximal de réponse Modbus ;
- timeout maître ;
- nombre maximal de retries ;
- backoff ;
- délai de résynchronisation ;
- fréquence maximale de requêtes ;
- priorité entre erreurs simultanées de transport.

Ces points sont conservés comme `NOT_DEFINED` et ne donnent lieu à aucun test de conformité V1 autonome.

## 3. Distinction essentielle

Le CRC métier/configuration éventuellement défini dans les blocs applicatifs n'est pas le CRC de trame Modbus RTU. Un oracle métier de CRC ne doit jamais être réutilisé pour conclure sur le traitement d'une trame RTU corrompue.

De même, les accès invalides définis par `charte_typage.md` §14 concernent une requête Modbus syntaxiquement exploitable visant un accès interdit par le mapping. Ils ne définissent pas le comportement d'une trame physiquement ou syntaxiquement corrompue.

## 4. Doctrine

FT-RBT-05 interdit d'importer silencieusement les usages génériques Modbus comme exigences propres au TR2. Si un futur référentiel normatif externe doit devenir applicable au produit, il devra être explicitement référencé et intégré à la hiérarchie normative du projet.

## 5. Artefacts

- `source/FT-RBT-05_source.md` ;
- `detaille/FT-RBT-05_detaille.md` ;
- `detaille/FT-RBT-05_matrice_couverture.csv`.

## 6. Statut

Sous-famille reconstruite pour revue. Gel interdit avant validation explicite et passe croisée finale FT-RBT.
