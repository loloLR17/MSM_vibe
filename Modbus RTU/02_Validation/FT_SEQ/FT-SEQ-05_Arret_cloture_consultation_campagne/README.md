# FT-SEQ-05 — Arrêt, clôture et consultation de campagne

## 1. Objet

Valider la chaîne nominale complète STOP : acquisition/campagne en cours → STOP réussi → acquisition arrêtée → campagne mise en cohérence/clôturée → consultation de ses métadonnées finales B6.

## 2. Résultat d'audit

La V1 définit suffisamment cette chaîne pour créer un oracle FT-SEQ propriétaire :

`campagne en cours` → `STOP réussi` → `acquisition arrêtée` → `campagne clôturée/cohérente` → `entrée B6 consultable`.

Un test propriétaire est créé :
- `TT-SEQ-CAMP-002` — arrêt nominal, clôture et consultation de campagne.

## 3. Couverture

- `COVERED` propriétaire FT-SEQ : 1
- `CONDITIONAL` : 0
- `DELEGATED` : 6
- `TRACE_ONLY` : 2
- `NOT_DEFINED` : 4

## 4. Délégations

- condition, acceptation/refus et résultat STOP : FT-CMD-06 ;
- STOP → acquisition arrêtée et campagne clôturée/cohérente : FT-INT-04 ;
- navigation et invariants internes B6 : FT-BLK-05 ;
- structure, accès et domaines : FT-STR / FT-ACC / FT-LIM.

FT-BLK-05 confirme notamment que la sélection B6 par index est normative, qu'une campagne valide a un `campaign_id != 0`, mais que l'égalité exacte universelle de durée n'est pas définie. 

## 5. Observabilité bornée

Le Bloc 5 annonce comme effets de STOP : vidage des buffers et fermeture des fichiers. Ces effets sont normatifs, mais la V1 ne fournit pas de registre Modbus servant de témoin direct et indépendant de chacun.

Ils restent donc `TRACE_ONLY` au niveau de leur observation directe. L'oracle fonctionnel exploitable est la mise en cohérence/clôture de la campagne exposée par B6.

## 6. Limites V1 conservées

FT-SEQ-05 n'impose pas :
- `campaign_state = 3` comme état universel dans tous les contextes possibles, notamment erreur/corruption ;
- `end_timestamp == timestamp STOP` ;
- `duration_s == end_timestamp - start_timestamp` comme égalité universelle ;
- un délai maximal STOP→finalisation.

## 7. Frontières

- refus STOP/code 21 : FT-CMD-06 ;
- démarrage/ouverture : FT-SEQ-04 ;
- cycle nominal complet : FT-SEQ-06 ;
- refus puis reprise : FT-SEQ-07 ;
- reboot/persistance : FT-PER ;
- robustesse hostile : FT-RBT.

Voir `source/FT-SEQ-05_source.md` et `detaille/TT-SEQ-CAMP-002.md`.