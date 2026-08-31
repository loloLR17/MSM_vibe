# FT-CMD-06 — Source normative consolidée

## 1. Sources

- `Modbus RTU/01_Specification_source/bloc5.md` V1 ;
- `bloc1.md` pour l'état d'acquisition exposé ;
- `bloc7.md` pour l'état et le résultat d'autotest ;
- délégations FT-INT relatives aux effets inter-blocs.

## 2. START ACQUISITION — commande 3

Conditions normatives d'acceptation :
- configuration active valide ;
- support SD exploitable ;
- absence de défaut critique bloquant ;
- acquisition non déjà active.

Refus typiques explicitement définis :
- `22` : aucune configuration active valide ;
- `6` : SD absente ;
- `7` : mémoire insuffisante ;
- `8` : défaut critique actif ;
- `3` : état incompatible.

La réussite de la transaction relève de FT-CMD ; le passage effectif de `acquisition_state` et l'ouverture d'une campagne relèvent de FT-INT.

## 3. STOP ACQUISITION — commande 4

Condition normative : acquisition active.

Refus explicite :
- `21` : arrêt impossible, acquisition non active.

Les effets de fermeture de fichiers, vidage de buffers et cohérence de campagne restent à FT-INT.

## 4. SELFTEST — commande 5

Paramètre normatif :
- `param1 = 0` : autotest standard complet ;
- autre valeur : masque de sous-tests uniquement si cette extension est implémentée.

Résultats Bloc 5 cités :
- `3` état incompatible ;
- `10` timeout interne ;
- `11` échec autotest.

Le Bloc 7 définit `selftest_status` et les champs de résultat. Leur publication après exécution relève de FT-INT.

## 5. ACK défaut / alarme — commande 6

Paramètres :
- `param1` = code défaut ;
- `param2 = 0` : acquittement unitaire ;
- `param2 = 1` : acquittement global des défauts acquittables.

Conditions :
- défaut concerné présent ;
- défaut acquittable.

Refus :
- `16` : défaut non acquittable ;
- `2` : paramètre invalide.

Règle inter-blocs : l'acquittement ne supprime jamais la cause si elle reste présente. Cette observation appartient à FT-INT.

## 6. REFRESH indicateurs — commande 7

Effet normatif annoncé : recalcul / rafraîchissement des indicateurs synthétiques, sans modification de configuration.

FT-CMD-06 couvre la transaction de commande. Les indicateurs exacts concernés ne sont pas énumérés dans le Bloc 5 et le non-effet sur la configuration est contrôlé en FT-INT.

## 7. Dettes / réserves V1

À conserver pour le rapport final :
- pas de priorité explicitement normée entre plusieurs causes simultanées de refus START ;
- masque SELFTEST `param1 != 0` conditionné à une extension non obligatoire ;
- pas de table exhaustive de `cmd_result_detail` pour SELFTEST ou ACK ;
- périmètre exact des indicateurs concernés par REFRESH non défini dans le Bloc 5.
