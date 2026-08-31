# FT-SEQ-07 — Référentiel source : refus puis reprise de séquence

## 1. Objet

Valider les chaînes V1 dans lesquelles une opération est d'abord refusée pour une cause normative explicitement identifiable, puis où cette cause est corrigée par une action elle-même normative avant une nouvelle tentative réussie.

FT-SEQ-07 ne possède ni le refus isolé, ni son code résultat : ils restent FT-CMD. FT-SEQ-07 possède la continuité fonctionnelle `refus attendu → correction normative → nouvelle tentative → succès et effets attendus`.

## 2. Critères d'admission

Une chaîne de reprise n'est admise dans FT-SEQ-07 que si :
1. la cause de refus est explicitement définie par V1 ;
2. le code/résultat attendu est couvert par FT-CMD ;
3. une action corrective est explicitement supportée par V1 ;
4. après correction, les préconditions de la nouvelle tentative peuvent être établies sans hypothèse supplémentaire ;
5. le succès final et ses effets sont observables par des oracles gelés.

Une simple répétition identique de commande, une perte de trame, une temporisation agressive ou une récupération après reboot ne sont pas des scénarios FT-SEQ-07.

## 3. Chaînes retenues

### SEQ07-R01 — START sans configuration active valide → correction → succès

- Classification : `COVERED`.
- Test : `TT-SEQ-REC-001`.
- Chaîne : `START sans configuration active valide` → refus FT-CMD-06 code `22` → préparer/appliquer une configuration valide via FT-SEQ-02 → nouvelle tentative START → succès → acquisition/campagne établies via FT-SEQ-04.

### SEQ07-R02 — SYNC TIME sans temps préparé → correction → succès

- Classification : `COVERED`.
- Test : `TT-SEQ-REC-002`.
- Chaîne : `SYNC TIME sans temps préparé` → refus FT-CMD-05 code `19` → préparer une référence temporelle B2 → nouvelle tentative SYNC TIME → succès → effets temporels via FT-SEQ-03/FT-INT-01.

### SEQ07-R03 — STOP acquisition inactive → établir état valide → succès

- Classification : `COVERED`.
- Test : `TT-SEQ-REC-003`.
- Chaîne : `STOP acquisition inactive` → refus FT-CMD-06 code `21` → établir les préconditions START → START réussi → acquisition active/campagne en cours → STOP réussi → acquisition arrêtée/campagne clôturée.
- Remarque : la correction n'est pas « annuler » le refus ; elle établit un contexte où STOP a désormais un sens normatif.

### SEQ07-R04 — APPLY CONFIG pendant acquisition → arrêt → nouvelle application réussie

- Classification : `COVERED`.
- Test : `TT-SEQ-REC-004`.
- Chaîne : acquisition active + configuration préparée applicable → APPLY CONFIG → refus FT-CMD-05 code `5` → STOP réussi → acquisition arrêtée → nouvelle tentative APPLY CONFIG → succès → configuration active cohérente.

## 4. Délégations communes

Pour les quatre chaînes :
- refus et code résultat : `DELEGATED` FT-CMD-05/06 ;
- transaction B5, corrélation et idempotence : `DELEGATED` FT-CMD ;
- correction configuration : `DELEGATED` FT-SEQ-02 et familles élémentaires ;
- correction temps : `DELEGATED` FT-SEQ-03 et familles élémentaires ;
- START/campagne : `DELEGATED` FT-SEQ-04 ;
- STOP/clôture : `DELEGATED` FT-SEQ-05 ;
- effets inter-blocs : `DELEGATED` FT-INT.

## 5. Exigences non admises / non définies

### SEQ07-R05 — Nombre maximal de tentatives après correction

- Classification : `NOT_DEFINED`.
- Aucune politique V1 générale de nombre maximal de retries n'est définie.

### SEQ07-R06 — Délai minimal/maximal avant nouvelle tentative

- Classification : `NOT_DEFINED`.
- Aucun backoff métier global n'est normé.

### SEQ07-R07 — Réutilisation obligatoire du même transaction_id pour le retry corrigé

- Classification : `NOT_DEFINED` comme politique FT-SEQ.
- La gestion des identifiants/idempotence appartient à FT-CMD ; une nouvelle intention corrigée ne reçoit pas ici une politique inventée.

### SEQ07-R08 — Priorité entre plusieurs causes simultanées de refus START

- Classification : `NOT_DEFINED`.
- Dette déjà conservée par FT-CMD-06.

### SEQ07-R09 — Reprise après horloge indisponible code 12

- Classification : `TRACE_ONLY` / non retenue comme test autonome V1.
- Le refus est normatif, mais V1 ne fournit pas de mécanisme séquentiel reproductible permettant au banc de rendre l'horloge disponible. Une chaîne corrective déterministe ne peut donc pas être imposée.

### SEQ07-R10 — Reprise après défaut critique START code 8

- Classification : `TRACE_ONLY` / non retenue comme test autonome V1.
- Le refus est normatif, mais la manière de supprimer la cause dépend du défaut et n'est pas définie comme une procédure générique V1.

### SEQ07-R11 — Reprise après SD absente/mémoire insuffisante codes 6/7

- Classification : `TRACE_ONLY` / non retenue comme test autonome V1.
- Les refus appartiennent à FT-CMD ; la restauration physique du support/capacité n'est pas une séquence métier générique normée par V1.

## 6. Doctrine de retry

Chaque nouvelle tentative corrigée doit être une transaction conforme à FT-CMD. FT-SEQ-07 ne redéfinit pas :
- les règles de `transaction_id` ;
- l'idempotence ;
- la corrélation ;
- la concurrence ;
- la progression interne `cmd_status`.

Le test doit démontrer que la cause initiale a été effectivement corrigée avant la nouvelle tentative. Il ne suffit pas de répéter jusqu'à obtenir un succès.

## 7. Anti-fabrication

Ne pas créer :
- retry automatique implicite du firmware ;
- nombre fixe de retries ;
- temporisation/backoff arbitraire ;
- réutilisation forcée d'un identifiant transactionnel ;
- procédure générique de récupération d'une SD, horloge ou faute critique ;
- reprise après reboot ;
- scénarios de pertes/retransmissions hostiles.

## 8. Frontières

- refus isolés : FT-CMD ;
- effets élémentaires après succès : FT-INT ;
- robustesse hostile : FT-RBT ;
- reboot/persistance : FT-PER.