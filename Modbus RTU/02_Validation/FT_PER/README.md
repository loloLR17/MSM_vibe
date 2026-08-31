# FT-PER — Persistance et reprise après reboot

## Objet

FT-PER valide exclusivement les propriétés V1 observables à travers une frontière réelle de redémarrage lorsque la spécification fournit un oracle discriminant.

La famille ne déduit jamais une politique de persistance à partir d'une intuition d'implémentation, du caractère RO/RW d'un champ, d'une donnée dite statique en fonctionnement normal ou d'une architecture supposée Flash/RAM.

## Doctrine

Question propriétaire FT-PER :

> Après une frontière réelle de redémarrage, quelle propriété V1 impose encore un état observable déterminé ?

Les classifications utilisées sont :

- `COVERED` — oracle V1 explicite et testable ;
- `CONDITIONAL` — oracle exploitable seulement sous une condition d'exécution clairement identifiée ;
- `DELEGATED` — propriété normative possédée par une autre famille ;
- `TRACE_ONLY` — observation utile sans verdict PASS/FAIL V1 ;
- `NOT_DEFINED` — comportement non défini par V1, interdit à inventer.

## Sous-familles

1. **FT-PER-01 — Reset logiciel et observables de redémarrage**
2. **FT-PER-02 — Identité et données explicitement persistantes**
3. **FT-PER-03 — Configuration et état préparé/actif**
4. **FT-PER-04 — Moteur transactionnel et historique après reboot**
5. **FT-PER-05 — État système, campagnes, statistiques et diagnostic**
6. **FT-PER-06 — Power cycle et reprise générale Modbus**

## Oracles V1 réellement retenus

Les principaux oracles post-reboot V1 sont volontairement peu nombreux :

- RESET SOFTWARE accepté/exécuté provoque un redémarrage logiciel contrôlé ;
- B1 `last_reset_cause = 2` après RESET SOFTWARE ;
- B7 `reset_cause = 2` après RESET SOFTWARE ;
- `device_id` est explicitement déclaré unique et persistent ;
- après power cycle contrôlé, les causes B1/B7 doivent identifier `power-on = 1` ;
- power-on et reset logiciel sont des causes normativement distinctes.

## Frontières de propriété

FT-PER ne reprend pas :

- structure, encodage et atomicité : FT-STR ;
- droits d'accès : FT-ACC ;
- domaines simples : FT-LIM ;
- invariants intra-bloc : FT-BLK ;
- moteur transactionnel nominal et commandes B5 : FT-CMD ;
- relations inter-blocs nominales : FT-INT ;
- séquences nominales complètes : FT-SEQ ;
- robustesse hostile hors propriété de reboot : FT-RBT.

## Dettes V1

La majorité des politiques classiques de reprise embarquée ne sont pas spécifiées en V1 : configuration active/préparée, mémoire d'idempotence, historique B5, acquisition, campagnes, diagnostics historiques, statistiques, valeurs initiales, reprise par cause de reset et timing de retour Modbus.

Elles restent visibles comme `NOT_DEFINED` et sont regroupées dans `EVOLUTIONS_CANDIDATES_V1_1.md`.

## Artefacts de clôture

- `MATRICE_COUVERTURE_FT_PER_V1.md` ;
- `AUDIT_FINAL_FT_PER_V1.md` ;
- `EVOLUTIONS_CANDIDATES_V1_1.md`.

## Statut

Les six sous-familles ont été reconstruites et validées individuellement. La famille est candidate au gel V1 après validation explicite de la passe croisée finale.
