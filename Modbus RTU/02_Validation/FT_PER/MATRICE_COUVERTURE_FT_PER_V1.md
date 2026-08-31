# FT-PER — Matrice de couverture consolidée V1

## 1. Synthèse

La famille FT-PER couvre les propriétés de persistance et de reprise après reboot uniquement lorsqu'un oracle V1 explicite existe.

Bilan consolidé après passe croisée finale :

- 69 exigences/classifications inventoriées ;
- 6 `COVERED` ;
- 1 `CONDITIONAL` ;
- 11 `DELEGATED` ;
- 6 `TRACE_ONLY` ;
- 45 `NOT_DEFINED` ;
- 6 scénarios de test FT-PER distincts.

La forte proportion de `NOT_DEFINED` est volontaire : elle reflète les limites réelles de la spécification V1 et évite de transformer des choix d'implémentation en exigences implicites.

## 2. Couverture par sous-famille

| Sous-famille | Périmètre | COVERED | CONDITIONAL | DELEGATED | TRACE_ONLY | NOT_DEFINED |
|---|---|---:|---:|---:|---:|---:|
| FT-PER-01 | RESET SOFTWARE, cause reset, uptime, reprise | 2 | 1 | 2 | 1 | 3 |
| FT-PER-02 | identité et persistance explicite | 1 | 0 | 2 | 0 | 7 |
| FT-PER-03 | configuration préparée/active | 0 | 0 | 1 | 1 | 10 |
| FT-PER-04 | moteur transactionnel/historique B5 | 0 | 0 | 2 | 1 | 9 |
| FT-PER-05 | état, campagnes, statistiques, diagnostic | 0 | 0 | 3 | 1 | 10 |
| FT-PER-06 | power cycle et reprise Modbus | 3 | 0 | 1 | 2 | 6 |
| **Total** |  | **6** | **1** | **11** | **6** | **45** |

## 3. Scénarios FT-PER distincts

- `TT-PER-B01B05B07-001` — RESET SOFTWARE puis cause de reset logiciel observable ;
- `TT-PER-B00-001` — persistance du `device_id` après RESET SOFTWARE ;
- `TT-PER-B04-001` — caractérisation configuration avant/après reboot ;
- `TT-PER-B05-001` — caractérisation moteur transactionnel avant/après reboot ;
- `TT-PER-B01B06B07-001` — caractérisation état/campagnes/diagnostic avant/après reboot ;
- `TT-PER-B01B07-002` — power cycle puis cause power-on observable.

Aucun identifiant n'est dupliqué entre les sous-familles.

## 4. Oracles V1 couverts

Les points réellement couverts sont :

- `B1.last_reset_cause = 2` après RESET SOFTWARE ;
- `B7.reset_cause = 2` après RESET SOFTWARE ;
- conservation du `device_id` à travers un RESET SOFTWARE contrôlé ;
- `B1.last_reset_cause = 1` après power cycle ;
- `B7.reset_cause = 1` après power cycle ;
- distinction normative `power-on = 1` / `reset logiciel = 2`.

Le redémarrage logiciel contrôlé lui-même est classé `CONDITIONAL` car son verdict FT-PER exige un banc capable de provoquer et d'établir réellement la frontière de reboot.

## 5. Principales zones NOT_DEFINED

Restent volontairement sans oracle PASS/FAIL V1 :

- persistance de la configuration active ;
- persistance de la configuration préparée ;
- état de `config_state` et des CRC/IDs après reboot ;
- mémoire d'idempotence B5 après reboot ;
- conservation de `cmd_last_*`, `cmd_active_*`, résultats et champs de requête ;
- récupération d'une commande interrompue ;
- état d'acquisition au boot et reprise automatique ;
- sort d'une campagne interrompue ;
- persistance/reconstruction de l'inventaire B6 ;
- persistance des diagnostics historiques et résultats d'autotest ;
- politique générale des statistiques/compteurs ;
- état système initial ;
- comportement selon watchdog, brown-out, reset externe ou firmware update ;
- durée maximale de boot et délai de reprise Modbus ;
- comportement Modbus pendant l'initialisation.

## 6. Délégations finales

- stabilité sans reboot et encodage : FT-STR ;
- accès : FT-ACC ;
- domaines : FT-LIM ;
- invariants intra-bloc : FT-BLK ;
- moteur transactionnel nominal, RESET SOFTWARE et RESET STATISTICS : FT-CMD ;
- relations inter-blocs nominales : FT-INT ;
- scénarios fonctionnels complets : FT-SEQ ;
- robustesse hostile : FT-RBT.

## 7. Conclusion

La matrice consolidée ne révèle aucun trou supplémentaire pouvant être fermé sans inventer de règle absente de V1.

La famille est candidate au gel V1 sous réserve de validation explicite de la passe finale et du document `AUDIT_FINAL_FT_PER_V1.md`.
