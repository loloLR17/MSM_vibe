# TR2-Wm — résultats locaux et interprétation

**L'indicateur global est calculé et vérifié sur toute l'archive.** Il décrit le
maximum des trois RMS d'accélération pondérée, en mm/s² nominaux. Il ne qualifie
ni le navire ni la sensation d'un passager. Instrument non étalonné, campagne
NON_CLASSÉE et **COMPARABILITE_BV_NON_ETABLIE** : aucun seuil ni classement.

| Indicateur principal MCU | Médiane | P95 | Maximum |
|---|---:|---:|---:|
| Fenêtres60s,365 FULL | 43,73 | 74,34 | 99,10 |
| Fenêtres120s,182 FULL | 43,89 | 74,22 | 87,78 |

Période entière valide : RMS XYZ23,379/18,643/43,874mm/s², maximum Z43,874.
La vitesse équivalente du même résultat vaut1,2285mm/s par0,028×a_w : **elle
dépend entièrement de a_w**, ce n'est pas une deuxième mesure indépendante ni
la vitesse physique non pondérée des études précédentes. Les valeurs non arrondies
et l'axe gagnant de chaque fenêtre restent dans le CSV/SQLite.

Le filtre est une réalisation originale de la fonction Wm accessible : trois
étages, limitation comprise une fois, sans reprendre le filtre test LP10.
Les points des tables sont vérifiés aux centres réels des tiers d'octave :
«80Hz» nominal signifie79,43Hz. Le filtre numérique diffère de la fonction
analogique de moins de0,47% en module sur1–80Hz ; cette erreur de discrétisation
n'est pas l'incertitude physique du TR2.

Sur182 paires de périodes identiques, la recombinaison des énergies60→120s
est exacte à4,5e−16 relatif. Rang des périodes Spearman0,983 ; le maximum des
deux minutes peut dépasser le RMS120s de30,1%, car la durée moyenne des niveaux
variables change. Une autre hypothèse de cadence change la mesure60s de−0,72
à−0,40%. La variation diagnostique de la borne basse de±20% change le RMS entre
−1,52% et+3,75% ; la pose, les axes, le gain et le bruit1Hz restent non qualifiés.

Les raccordements Wm sont lisses, pas un découpage fréquentiel rectangulaire.
Le diagnostic PSD Hann par blocs estime des parts hors1–80Hz non nulles
(médianes1,26% sous1Hz/2,49% au-dessus80Hz). Ce calcul approximatif inclut la fuite
de fenêtre ; on ne rajoute pas un deuxième passe-bande pour maquiller ces queues.
Les dérives/inclinaisons peuvent également contribuer ; ce n'est pas un filtre
capable de distinguer toutes les causes physiques de vibration.

L'épisode déjà connu de sorties nominales jusqu'à7,677g reste inclus dans le
principal. Exclure17600–17630s uniquement dans un diagnostic réduit le RMS de
période entière de0,036% ; cela ne prouve pas sa cause et ne justifie aucune
suppression dans l'archive. Les fenêtres affectées du diagnostic restent de
couverture réduite. La pleine échelle n'est pas modifiée : conserver8g selon
l'étude précédente, pas de preuve continue du registre sur les6h.

**Neuf tests ciblés et72 contre-calculs réels indépendants PASS.** Une première
sommation Parseval avait dépassé sa limite à1,039e−12 ; accumulation améliorée,
limite1e−12 conservée. Les RMS temporels/FFT concordent à9,03e−11 relatif maximum.
Le filtrage entier n'a pas été répété : agrégats vérifiés et contrôles seuls repris.

Après ces validations, BV2026 a été relu : sa voie RMS globale pondérée renvoie
à ISO20283-5, distincte des composantes PK. Catégorie/occupation du local,
axes/pose/conditions et identité exhaustive des références finales ne sont pas
établis pour cette campagne. Aucun repère GOST n'est substitué et aucun seuil
BV n'est sélectionné. Les relations historiques/projet2026 et les points publics
concordent numériquement, sans preuve de conformité à toutes les exigences
finales ISO/8041 ; cette réserve reste visible dans la méthode versionnée.

La source, les anciens catalogues et analyses sont intacts ; une seule copie
cumulative v5 contient les nouveaux résultats. Le digest RAM indépendant perdu
maintient la réserve R3. Aucun STM32, SD, COM ou Windows accédé, aucune acquisition
ou dépense. Prochaine étape : revue indépendante ChatGPT et clarification explicite
de la catégorie/mesure de référence avant une éventuelle comparaison BV.

[Courbes](index.html) · [Preuves](README.md).
