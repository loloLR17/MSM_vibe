# FT-CMD-05 — Source normative consolidée

## 1. Commande 1 — APPLY CONFIG

Le Bloc 5 définit les conditions d'acceptation suivantes :
- acquisition arrêtée ;
- configuration préparée complète ;
- contenu du Bloc 4 valide ;
- recalcul interne du CRC de la zone préparée conforme à `prepared_config_crc`.

Le Bloc 4 impose en outre qu'une configuration ne puisse être appliquée que si `config_state = VALIDE`.

Les codes de refus explicitement rattachés à cette commande sont :
- `3` état incompatible ;
- `5` acquisition en cours ;
- `20` configuration préparée incomplète ;
- `4` configuration invalide.

Règle de sûreté : le CRC préparé doit être recalculé au moment de l'application. En cas d'incohérence, l'application est refusée et un résultat d'invalidité/incohérence de configuration est exposé.

### Frontière FT-INT

FT-CMD-05 s'arrête à la décision transactionnelle et au résultat Bloc 5. Les effets réussis sur `config_state`, `active_config_id`, `active_config_crc` et autres champs du Bloc 4 restent FT-INT.

## 2. Commande 2 — SYNC TIME

Le Bloc 5 définit :
- heure préparée présente ;
- horloge disponible.

Refus :
- `19` synchronisation préparée absente ;
- `12` horloge non disponible.

Le Bloc 2 confirme que l'écriture de `prepared_time` ne modifie pas immédiatement l'horloge et que l'application effective passe uniquement par la commande Bloc 5.

### Frontière FT-INT

FT-CMD-05 valide l'acceptation/refus de la commande. Les effets sur `current_time`, `last_sync_time`, `prepared_time_status`, `time_status` et flags associés restent FT-INT.

## 3. Non-oracles

Ne pas imposer :
- une séquence intermédiaire précise de `cmd_status` ;
- un `cmd_result_detail` particulier, sauf texte normatif explicite ;
- une transition Bloc 2/4 dans les tests FT-CMD eux-mêmes ;
- une assimilation systématique de tout échec APPLY au code 4 si la cause dispose d'un code plus spécifique.

## 4. Point V1.1 potentiel

La phrase du Bloc 4 selon laquelle un CRC incohérent doit produire « une erreur de configuration invalide ou incohérente » reste moins précise que la table de résultats du Bloc 5, qui ne contient pas de code distinct « CRC incohérent ». Pour FT-CMD-05, le code 4 est retenu comme oracle de refus de configuration invalide, sans inventer un nouveau code.
