# FT-SEQ-06 — Référentiel source : cycle nominal complet de campagne

## 1. Objet

Valider, dans une seule exécution système, l'enchaînement nominal complet des opérations V1 nécessaires pour passer d'un contexte initial connu à une campagne correctement configurée, temporellement référencée, démarrée, exploitée, arrêtée puis consultée.

FT-SEQ-06 est le scénario d'intégration E2E principal de la famille FT-SEQ. Il ne duplique pas les oracles des sous-familles FT-SEQ-02 à 05 ni ceux des familles gelées : il valide leur continuité à l'échelle métier.

## 2. Sources et scénarios composés

Sources V1 principales : blocs 0, 1, 2, 4, 5, 6 et 7 selon les observables utilisés.

Scénarios FT-SEQ composés :
- FT-SEQ-01 : qualification initiale / établissement du contexte, sans ordre de lecture imposé ;
- FT-SEQ-02 : préparation et activation d'une configuration ;
- FT-SEQ-03 : préparation et synchronisation temporelle ;
- FT-SEQ-04 : démarrage et établissement d'une campagne ;
- FT-SEQ-05 : arrêt, clôture et consultation de campagne.

Oracles élémentaires délégués : FT-STR, FT-ACC, FT-LIM, FT-BLK, FT-INT et FT-CMD.

## 3. Chaîne E2E retenue

Le cycle nominal de référence est :

`qualification/contexte` → `préparation temporelle` → `SYNC TIME` → `préparation configuration` → `APPLY CONFIG` → `START` → `acquisition/campagne en cours` → `STOP` → `campagne clôturée/cohérente` → `consultation inventaire/diagnostic`.

Cette représentation décrit un scénario d'intégration choisi pour exercer dans une même exécution les dépendances normatives disponibles. Elle ne signifie pas que la V1 impose universellement cet ordre pour toute utilisation du produit.

En particulier :
- FT-SEQ-01 ne définit aucun handshake initial obligatoire ;
- la V1 n'impose pas une synchronisation horaire avant chaque START ;
- aucune exigence n'impose de lire B7 à une position précise du cycle ;
- l'ordre SYNC TIME / APPLY CONFIG n'est pas transformé en dépendance normative générale en l'absence de règle V1 correspondante.

## 4. Exigences FT-SEQ-06

### SEQ06-R01 — Cycle nominal complet de campagne

- Classification : `COVERED`.
- Propriétaire : FT-SEQ-06.
- Test : `TT-SEQ-SYS-001`.
- Exigence : un scénario système réunissant les opérations normatives de préparation temporelle, activation de configuration, démarrage, campagne en cours, arrêt et consultation finale doit pouvoir être exécuté sans rupture de cohérence entre les étapes lorsque toutes leurs préconditions respectives sont satisfaites.
- Oracle FT-SEQ-06 : continuité E2E et absence de contradiction entre les états de sortie d'une étape et les préconditions/observables de l'étape suivante.

### SEQ06-R02 — Qualification initiale comme préambule obligatoire et ordonné

- Classification : `NOT_DEFINED`.
- Justification : FT-SEQ-01 a établi qu'aucun ordre B0→B1→B2→B4→B5→B6→B7 n'est normé.

### SEQ06-R03 — Préparation/activation configuration

- Classification : `DELEGATED`.
- Propriétaire : FT-SEQ-02 pour la chaîne ; familles élémentaires pour les oracles internes.

### SEQ06-R04 — Préparation/synchronisation temporelle

- Classification : `DELEGATED`.
- Propriétaire : FT-SEQ-03.

### SEQ06-R05 — START et ouverture de campagne

- Classification : `DELEGATED`.
- Propriétaire : FT-SEQ-04.

### SEQ06-R06 — STOP, clôture et consultation

- Classification : `DELEGATED`.
- Propriétaire : FT-SEQ-05.

### SEQ06-R07 — Synchronisation temporelle obligatoire avant chaque START

- Classification : `NOT_DEFINED`.
- Justification : aucune règle V1 identifiée n'établit cette dépendance universelle.

### SEQ06-R08 — Ordre universel SYNC TIME avant APPLY CONFIG

- Classification : `NOT_DEFINED`.
- Justification : les deux chaînes ont leurs propres préconditions, mais aucune dépendance normative générale entre elles n'est définie.

### SEQ06-R09 — Consultation diagnostic B7 obligatoire après STOP

- Classification : `NOT_DEFINED`.
- Justification : B7 fournit des informations de diagnostic, mais aucune séquence V1 n'impose cette lecture comme condition de clôture d'une campagne.

### SEQ06-R10 — Cohérence de bout en bout sans égalités non normées

- Classification : `COVERED` dans `TT-SEQ-SYS-001`.
- Exigence : la preuve E2E doit conserver les limites gelées, notamment aucune égalité B1/B6 inventée, aucune égalité temporelle bit-à-bit entre transactions séparées, aucun compteur supposé immédiatement incrémenté.

### SEQ06-R11 — Délai maximal du cycle complet

- Classification : `NOT_DEFINED`.
- Justification : aucune durée maximale globale du cycle n'est normée.

### SEQ06-R12 — Persistance du cycle après reboot

- Classification : `DELEGATED`.
- Propriétaire : FT-PER.

### SEQ06-R13 — Résilience du cycle à pertes, répétitions agressives ou délais hostiles

- Classification : `DELEGATED`.
- Propriétaire : FT-RBT.

## 5. Philosophie du test E2E

`TT-SEQ-SYS-001` n'est pas une copie concaténée de tous les tests détaillés précédents.

Il doit :
- choisir un jeu de données nominal cohérent ;
- exécuter les opérations principales dans une même session ;
- utiliser les résultats des étapes précédentes comme contexte réel des suivantes ;
- contrôler des jalons représentatifs suffisants pour démontrer la continuité ;
- référencer les tests/familles propriétaires lorsqu'un jalon échoue ;
- éviter la multiplication combinatoire de variantes déjà couvertes ailleurs.

## 6. Ordre du scénario de référence

Pour rendre le test reproductible, `TT-SEQ-SYS-001` adopte l'ordre pratique suivant :
1. établir le contexte initial ;
2. préparer et synchroniser le temps ;
3. préparer et appliquer une configuration ;
4. démarrer l'acquisition ;
5. constater acquisition et campagne en cours ;
6. arrêter l'acquisition ;
7. retrouver et consulter la campagne clôturée ;
8. consulter les diagnostics/états utiles à la preuve finale.

Cet ordre est un **ordre de test**, pas une nouvelle exigence protocolaire générale. Seules les dépendances explicitement normatives à l'intérieur de chaque chaîne deviennent des oracles.

## 7. Anti-fabrication

Ne pas déduire du scénario E2E :
- un handshake initial obligatoire ;
- une obligation de synchroniser avant chaque acquisition ;
- un ordre universel entre synchronisation et configuration ;
- une lecture B7 obligatoire après chaque campagne ;
- `B1.active_campaign_id == B6.campaign_id` ;
- `total_campaign_count + 1` immédiatement ;
- une égalité stricte entre timestamps issus de transactions distinctes ;
- un délai maximal du cycle ;
- une politique de reboot ou de robustesse hostile.

## 8. Frontière avec FT-SEQ-07

FT-SEQ-06 ne provoque pas volontairement de refus. Les cycles `erreur normative → correction → reprise → succès` appartiennent à FT-SEQ-07.