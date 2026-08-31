# FT-PER-01 — Procédure détaillée

## TT-PER-B01B05B07-001 — RESET SOFTWARE puis cause de reset logiciel observable

### Classification d'exécution

`CONDITIONAL` — le banc doit permettre d'exécuter un RESET SOFTWARE réel ou simulé de façon contrôlée puis de reprendre les lectures Modbus après redémarrage.

### Objectif

Vérifier qu'un RESET SOFTWARE accepté et réellement exécuté produit un redémarrage identifiable comme **reset logiciel** dans les observables normatifs de B1 et B7, sans inventer de contrainte temporelle de boot.

### Sources normatives

- Bloc 5 §8.10 — commande 10 RESET SOFTWARE : effet `redémarrage logiciel contrôlé` ;
- Bloc 1 §6.2 — `last_reset_cause = 2` : Reset logiciel ;
- Bloc 7 §8.3 — `uptime_s` depuis dernier reset ;
- Bloc 7 §8.4 — `reset_cause = 2` : reset logiciel ;
- FT-CMD-07 gelée pour l'acceptation transactionnelle de la commande ;
- FT-INT-05 gelée pour les relations générales B1↔B7.

### Préconditions

- capteur ou simulateur dans un état où RESET SOFTWARE peut être accepté selon FT-CMD-07 ;
- acquisition arrêtée ;
- aucune opération critique connue comme non terminée ;
- clé de confirmation et transaction préparées selon FT-CMD-07 ;
- accès avant reboot à B1, B5 et B7 ;
- moyen d'essai capable de détecter le franchissement du reboot et de reprendre les lectures après retour du serveur Modbus ;
- aucune autre cause de reset ne doit être injectée pendant le scénario.

Les préconditions transactionnelles sont utilisées comme préparation du stimulus ; leur verdict appartient à FT-CMD.

### Données à capturer avant reboot

- `B1.last_reset_cause` ;
- uptime B1 si exploitable ;
- `B7.uptime_s` ;
- `B7.reset_cause` ;
- transaction RESET SOFTWARE et résultat B5 disponible avant rupture de communication, selon l'oracle FT-CMD ;
- timestamp du banc à la dernière réponse Modbus valide avant reboot.

### Étapes

1. Lire et enregistrer les observables B1 et B7 avant le reset.
2. Préparer puis soumettre RESET SOFTWARE selon FT-CMD-07 avec un `transaction_id` dédié.
3. Vérifier que la commande est effectivement acceptée/exploitable selon FT-CMD ; si ce n'est pas le cas, ne pas prononcer de verdict FT-PER.
4. Observer le franchissement effectif du redémarrage par le moyen de banc disponible. La perte de communication peut être tracée mais n'est pas, seule, l'oracle de type de reset.
5. Reprendre les interrogations Modbus lorsque le capteur répond de nouveau. Aucun timeout normatif n'est appliqué.
6. À la première fenêtre de lecture post-reboot exploitable, lire `B1.last_reset_cause`.
7. Lire `B7.reset_cause` et `B7.uptime_s`.
8. Enregistrer également l'uptime B1 si disponible dans la campagne de lecture.
9. Enregistrer la durée observée entre dernière réponse pré-reboot et première réponse post-reboot comme mesure de caractérisation uniquement.

### Résultat attendu normatif

- `B1.last_reset_cause = 2` ;
- `B7.reset_cause = 2`.

Ces deux attentes sont appliquées séparément comme sémantique normative de chaque champ après le RESET SOFTWARE exécuté.

### Observations TRACE_ONLY

- évolution de `B7.uptime_s` avant/après ;
- uptime B1 avant/après ;
- éventuelle proximité B1/B7 ;
- durée d'indisponibilité Modbus ;
- nombre de tentatives nécessaires avant première réponse post-reboot.

Aucune borne ni égalité stricte n'est imposée à ces observations.

### PASS

Le stimulus RESET SOFTWARE est confirmé exploitable, un redémarrage a effectivement eu lieu, puis les deux observables normatifs post-reboot identifient la cause `2 = reset logiciel`.

### FAIL

Le stimulus RESET SOFTWARE est confirmé accepté/exécuté et le redémarrage a eu lieu, mais :
- B1 expose une cause différente de `2`, ou
- B7 expose une cause différente de `2`.

Chaque divergence doit être enregistrée séparément afin de ne pas masquer quel bloc porte la non-conformité.

### NOT_EXECUTABLE

- le moyen d'essai ne permet pas de provoquer ou simuler le reboot de façon contrôlée ;
- la communication ne peut pas être reprise de manière exploitable après reboot ;
- une autre cause de reset interfère avec le scénario ;
- il est impossible d'établir que le RESET SOFTWARE a réellement été accepté/exécuté.

### Pas de FAIL FT-PER dans les cas suivants

- reboot jugé « trop long » : aucune limite V1 ;
- première valeur d'uptime différente de zéro : aucune valeur initiale observable imposée ;
- uptime post-reboot supérieur à une ancienne lecture pré-reboot prise très tôt : aucune comparaison simple obligatoire n'est définie ;
- différence B1/B7 d'uptime : aucune tolérance normative ;
- contenu inattendu de `cmd_last_*` ou `cmd_active_*` : relève de FT-PER-04 et reste actuellement non défini.

---

## Cas volontairement non instanciés dans FT-PER-01

### RESET SOFTWARE refusé sans clé ou acquisition active

Déjà propriétaire FT-CMD-07. Aucun test FT-PER supplémentaire.

### RESET SOFTWARE pendant opération critique

La condition existe, mais la notion d'opération critique et le code exact restent partiellement non définis dans FT-CMD. FT-PER-01 ne la redéfinit pas.

### Délai maximal de reboot

`NOT_DEFINED`.

### Délai maximal de reprise Modbus

`NOT_DEFINED`.

### État initial B5 après reboot

`NOT_DEFINED`, reporté à FT-PER-04 pour documentation de la dette.

### Persistance du résultat de la commande RESET SOFTWARE

`NOT_DEFINED`, reportée à FT-PER-04.