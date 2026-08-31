# FT-INT-05 — État et diagnostic transversal

## 1. Objet

FT-INT-05 vérifie les relations fonctionnelles V1 qui traversent les blocs d'état, de temps, de commandes et de diagnostic, principalement B1, B2, B5 et B7.

Cette sous-famille ne reconstruit ni les invariants internes des blocs, ni le moteur transactionnel B5.

## 2. Périmètre

Sont couverts ou classifiés :

- publication dans B7 des effets d'un autotest déclenché par B5 ;
- conservation de la cause d'un défaut après acquittement tant que la cause reste présente ;
- base temporelle B2 de `B7.last_fault_timestamp` ;
- observables dupliqués B1/B7 : uptime, cause de reset et température interne ;
- limites normatives des relations B1↔B7 et B6↔B7.

## 3. Doctrine

Une relation inter-blocs ne devient un oracle PASS/FAIL que si la V1 la formule suffisamment pour être déterministe.

En particulier, FT-INT-05 n'invente pas :

- de table exhaustive entre `B1.system_status` et `B7.system_health_status` ;
- d'égalité exhaustive entre les bitfields défaut B1 et B7 ;
- de correspondance normative entre l'état stockage B6 et les défauts B7 ;
- de tolérance temporelle absente de la V1 ;
- de sémantique supplémentaire pour les codes de résultat d'autotest.

Les compléments métier informatifs de B7 ne sont pas utilisés comme oracles V1.

## 4. Répartition avec les autres familles

- FT-STR : structure, types, ordre des mots et lectures cohérentes ;
- FT-ACC : caractère RO de B7 et permissions ;
- FT-LIM : domaines et codes ;
- FT-BLK : invariants internes, notamment monotonie d'uptime ;
- FT-CMD : submit, transaction_id, états/résultats du moteur et refus des commandes SELFTEST/ACK ;
- FT-PER : comportement après redémarrage ;
- FT-INT-05 : effets observables de commandes sur B7 et cohérences réellement inter-blocs.

## 5. Tests actifs

- `TT-INT-B05B07-001` — Publication d'un autotest réussi ;
- `TT-INT-B05B07-002` — Publication d'un autotest en échec ;
- `TT-INT-B01B05B07-001` — Acquittement sans disparition de la cause ;
- `TT-INT-B02B07-001` — Base temporelle du dernier défaut ;
- `TT-INT-B01B07-001` — Observables dupliqués B1/B7, contrôle croisé non bloquant.

Les tests nécessitant une injection déterministe de résultat d'autotest ou de défaut sont conditionnels au moyen d'essai.

## 6. Artefacts

- `source/FT-INT-05_source.md`
- `detaille/FT-INT-05_detaille.md`
- `detaille/FT-INT-05_matrice_couverture.csv`

## 7. Statut

Sous-famille reconstruite sur le cadrage validé. Le gel de FT-INT reste interdit avant matrice consolidée et audit croisé final B0→B7.