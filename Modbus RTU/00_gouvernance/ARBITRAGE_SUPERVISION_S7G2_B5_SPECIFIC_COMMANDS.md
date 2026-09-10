# Arbitrage supervision S7-G2 — Commandes B5 à contrat spécifique

## Statut

Décision d’implémentation supervision PC, tranche S7-G2.

## Objet

Étendre le write path Web S7-G1 aux commandes V1 `ACKNOWLEDGE_FAULT` et `SOFTWARE_RESET` sans exposer les registres ou paramètres B5 bruts au navigateur.

## Références normatives

- Bloc 5 V1 : commande 6, `param1` = code défaut, `param2 = 0` acquittement unitaire, `param2 = 1` acquittement global des défauts acquittables.
- Bloc 5 V1 : commande 10 protégée par `confirm_key = 0xA55A`.
- L’acquittement ne supprime pas la cause d’un défaut encore présent.
- `RESET_STATISTICS` reste hors périmètre : le scope firmware resettable est `EMPTY`.

## Décisions

- Le même endpoint `POST /api/v1/devices/{deviceId}/commands` est conservé.
- Le navigateur exprime une intention sémantique ; il ne fournit jamais `cmd_request_code`, `param1`, `param2`, `param3` ou `confirm_key` directement.
- `AcknowledgeFault` exige un mode explicite :
  - unitaire : `acknowledgeAll=false` et `faultCode` obligatoire ;
  - global : `acknowledgeAll=true` et `faultCode` absent.
- `SoftwareReset` exige `confirmProtectedCommand=true` dans la requête Web. Cette confirmation est une barrière UX/API ; la valeur normative `0xA55A` reste générée exclusivement dans l’adaptateur Service.
- Les champs spécifiques sont interdits sur les autres commandes afin d’éviter toute donnée silencieusement ignorée.
- Le mécanisme `requestIdentity`, la sérialisation par `_queueGate`, le journal B5 et l’autorité `SupervisionOperationalFacade.QueueCommandAsync(...)` de S7-G1 restent inchangés.
- Un HTTP 202 signifie uniquement « transaction préparée et mise en file » ; il ne prouve ni prise en compte ni succès côté TR2.

## Mapping Service

- `AcknowledgeFault` unitaire -> `B5CommandIntent(6, faultCode, 0, 0, 0)`.
- `AcknowledgeFault` global -> `B5CommandIntent(6, 0, 1, 0, 0)` ; `param1` n’est pas interprété en mode global.
- `SoftwareReset` confirmé -> `B5CommandIntent(10, 0, 0, 0, 0xA55A)`.

## Sécurité / exposition

L’absence d’authentification reste la politique S7-A. Par conséquent, si `allowRemote=true` est explicitement activé, tout client pouvant atteindre l’interface HTTP peut demander les commandes autorisées, y compris `SOFTWARE_RESET` après confirmation API. Le défaut reste loopback et aucune écoute distante n’est activée implicitement.

## Hors périmètre

- boutons et dialogues IHM ;
- `RESET_STATISTICS` ;
- écriture B6 ;
- retry automatique ;
- résolution `Ambiguous` ;
- authentification, rôles ou ACL ;
- exposition de paramètres B5 bruts.

## Validation attendue

- compatibilité des commandes S7-G1 ;
- validation stricte des formes unitaire/globale d’acquittement ;
- refus du reset sans confirmation explicite ;
- transmission de l’intention sémantique au sink ;
- maintien du refus de `ResetStatistics`.
